#include "MV_Map.h"
#include "GameController.h"
#include "PaperSpriteActor.h"
#include "Quest.h"
#include "MV_QuestGiver.h"
#include "PaperSpriteComponent.h"
#include "MV_Area.h"
#include "PublicEnums.h"
#include "MV_Enemy.h"
#include "MV_EntityBase.h"
#include "Components/BoxComponent.h"
#include "MV_Village.h"
#include "MVSevenCharacter.h"
#include <Kismet\KismetMathLibrary.h>
#include <Kismet\GameplayStatics.h>

AMV_Map::AMV_Map()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	SetActorTickInterval(2.0f);
}

void AMV_Map::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < Areas.Num(); i++)
	{
		Areas[i]->Map = this;
		Areas[i]->UniqueAreaID = i;
	}

	const UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	const TArray<FAMV_EntityBaseInfo>& EntitiesToSpawn = GameController->RetrieveActiveEntities();
	const TArray<FAMV_QuestInfo>& QuestGiversToSoawn = GameController->RetrieveActiveQuests();
	Time = GameController->RetrieveTime();
	MVSevenCharacter = Cast<AMVSevenCharacter>(UGameplayStatics::GetActorOfClass(this, AMVSevenCharacter::StaticClass()));

	if (EntitiesToSpawn.Num() == 0)
	{
		// First start
		GenerateEntites();
	}
	else
	{
		// Any other Start (coming from Mission, loaded game)
		LoadSavedEntities(EntitiesToSpawn);
		LoadSavedQuests(QuestGiversToSoawn);
		MVSevenCharacter->SetActorLocation(GameController->GetPlayerStats().Position);
	}
}

void AMV_Map::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	++Time.Hour;

	switch (Time.Hour)
	{
		case 5:
			Time.DayPart = EDayPart::Morning;
			OnDayPeriodChange.Broadcast(Time.DayPart);
			break;
		case 10:
			Time.DayPart = EDayPart::Day;
			OnDayPeriodChange.Broadcast(Time.DayPart);
			break;
		case 17:
			Time.DayPart = EDayPart::Evening;
			OnDayPeriodChange.Broadcast(Time.DayPart);
			break;
		case 20:
			Time.DayPart = EDayPart::Night;
			OnDayPeriodChange.Broadcast(Time.DayPart);
			break;
		default:
			break;
	}

	if (Time.Hour == 10)
	{
		GenerateQuestGiver(MVSevenCharacter->GetCurrentAreaID());
	}

	if (Time.Hour == 24)
	{
		++Time.Day;
		Time.Hour = 0;

		if (GetActiveEnemies() < 4)
		{
			// just for DEBUG
		}

		if (Time.Day == 31)
		{
			++Time.Month;
			Time.Day = 1;

			if (Time.Month == 12)
			{
				++Time.Year;
				Time.Month = 1;
			}
		}
	}

	UE_LOG(LogTemp, Error, TEXT("[AMV_Map].Tick hour - %d, day - %d month - %d, year - %d"), Time.Hour, Time.Day, Time.Month, Time.Year);
}

FVector AMV_Map::GetRandomPointOnMap(const AMV_Area* const Area, const bool bShift, const int32 OverlapRadius) const
{
	bool bCanOverlap = (OverlapRadius == -1);
	FVector RandomPoint;
	const FBoxSphereBounds SpriteBounds = GetRenderComponent()->CalcBounds(GetRenderComponent()->GetComponentTransform());
	const FVector Origin = Area ? Area->BoxComponent->Bounds.Origin : SpriteBounds.Origin;
	const FVector Extent = Area ? Area->BoxComponent->Bounds.BoxExtent : SpriteBounds.BoxExtent;

	for (int i = 0; i < 20; i++)
	{
		RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
		RandomPoint.Z = SpriteBounds.Origin.Z + (bShift ? 1 : 0);

		if (bCanOverlap)
		{
			// Return first occurence
			return RandomPoint;
		}
		else if (!IsOverlappingAnyEntity(RandomPoint, OverlapRadius))
		{
			// Not overlapping with anything, return
			return RandomPoint;
		}

	}
	UE_LOG(LogTemp, Error, TEXT("[AMV_Map].GetRandomPointOnMap Unable to find non-overlapping area, returning last found"));
	return RandomPoint;
}

const TObjectPtr<AMVSevenCharacter> AMV_Map::GetMVSevenCharacter() const
{
	return MVSevenCharacter;
}

const AMV_EntityBase* AMV_Map::GenerateEntity(const int8 Index, EMissionType MissionType)
{
	FVector RandomPoint = GetRandomPointOnMap((Index >= 0 ? Areas[Index] : nullptr), true, 100);
	
	UMissionDA* NewMission = NewObject<UMissionDA>();

	if (MissionType == EMissionType::NotProvided)
	{
		uint32 RandomMissionIndex = FMath::RandRange(0, AvailableMissions.Num() - 1);
		NewMission = AvailableMissions[RandomMissionIndex];
	}
	else if (MissionType == EMissionType::Enemy)
	{
		NewMission = GenerateRandomEnemyMission(Index);
	}
	else if (MissionType == EMissionType::LiberatePlace)
	{
		uint32 RandomMissionIndex = FMath::RandRange(0, AvailableVillages.Num() - 1);
		const UMissionDA* VillageTemplate = AvailableVillages[RandomMissionIndex];

		NewMission->Name = VillageTemplate->Name;
		NewMission->Description = VillageTemplate->Description;
		NewMission->Image = VillageTemplate->Image;
		NewMission->MissionCompleteImage = VillageTemplate->MissionCompleteImage;
		NewMission->MissionType = VillageTemplate->MissionType;
		NewMission->EnemiesToSpawn = VillageTemplate->EnemiesToSpawn;
		NewMission->Reward = VillageTemplate->Reward;
		NewMission->MissionStatus = EStatus::Initialized;
		NewMission->AreaIndex = Index;
		NewMission->SpecialCharacter = nullptr;
	}

	GeneratedMissions.Add(NewMission);
	return SpawnEntity(FAMV_EntityBaseInfo(RandomPoint, NewMission));
}

void AMV_Map::GenerateQuestGiver(const int8 Index)
{
	// Creates new Quest (alongside with a new Mission - goal of the Quest).

	const FVector RandomPoint = GetRandomPointOnMap((Index >= 0 ? Areas[Index] : nullptr), true, 100);
	const AMV_EntityBase* NewEntity = GenerateEntity(Index);
	FString NewName = "This is your mission quest: " + NewEntity->GetMissionDA()->GetName();

	UQuest* Quest = NewObject<UQuest>();
	Quest->Name = FName(*NewName);
	Quest->Mission = NewEntity->GetMissionDA();

	SpawnQuestGiver(FAMV_QuestInfo(RandomPoint, Quest));
}

UMissionDA* AMV_Map::GenerateRandomEnemyMission(int Index) const
{
	UMissionDA* EnemyTemplate = AvailableEnemies[0];
	UMissionDA* NewEnemyMission = NewObject<UMissionDA>();

	NewEnemyMission->Name = "RandomGuy"; // TODO: take from DataTable
	NewEnemyMission->Description = "Random Guy Description"; // TODO: take from DataTable
	NewEnemyMission->Image = EnemyTemplate->Image; // TODO: take from DataTable
	NewEnemyMission->MissionCompleteImage = EnemyTemplate->MissionCompleteImage; // TODO: take from DataTable
	NewEnemyMission->MissionType = EnemyTemplate->MissionType; // TODO: take from DataTable
	NewEnemyMission->EnemiesToSpawn = EnemyTemplate->EnemiesToSpawn; // TODO: take from DataTable
	NewEnemyMission->Reward = EnemyTemplate->Reward; // TODO: take from DataTable
	NewEnemyMission->MissionStatus = EStatus::Initialized; // TODO: take from DataTable
	NewEnemyMission->AreaIndex = Index; // TODO: take from DataTable
	NewEnemyMission->SpecialCharacter = nullptr; // TODO: take from DataTable

	return NewEnemyMission;
}

void AMV_Map::GenerateEntites()
{
	for (int i = 0; i < Areas.Num() - 1; i++)
	{
		// Generate Enemies
		for (int j = 0; j < 3; j++)
		{
			GenerateEntity(-1, EMissionType::Enemy);
		}

		// Generate Villages
		GenerateEntity(i, EMissionType::LiberatePlace);

		// Generate Random Entities around ...
		for (int j = 0; j < 20; j++)
		{
			//GenerateEntity(i);
		}
	}
}

void AMV_Map::SpawnQuestGiver(const FAMV_QuestInfo& QuestGiverToSpawn)
{
	const FTransform T{ FRotator(0, -180, -90), QuestGiverToSpawn.Position, FVector(1,1,1) };

	AMV_QuestGiver* MV_QuestGiver = GetWorld()->SpawnActorDeferred<AMV_QuestGiver>(QuestGiverClass, T, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	MV_QuestGiver->Quest = QuestGiverToSpawn.Quest;
	MV_QuestGiver->FinishSpawning(T);

	ActiveQuestGivers.Add(MV_QuestGiver);
}

const AMV_EntityBase* AMV_Map::SpawnEntity(const FAMV_EntityBaseInfo& EntityToSpawn)
{
	const FTransform T{ FRotator(0, -180, -90), EntityToSpawn.Position, FVector(1,1,1) };
	AMV_EntityBase* NewEntity{ nullptr };
	if (EntityToSpawn.MissionDA->MissionType == EMissionType::LiberatePlace)
	{
		NewEntity = GetWorld()->SpawnActorDeferred<AMV_Village>(VillageClass, T, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	}
	else if (EntityToSpawn.MissionDA->MissionType == EMissionType::EnemyCamp || EntityToSpawn.MissionDA->MissionType == EMissionType::Enemy)
	{
		NewEntity = GetWorld()->SpawnActorDeferred<AMV_Enemy>(EnemyClass, T, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	}
	else
	{
		NewEntity = GetWorld()->SpawnActorDeferred<AMV_EntityBase>(MissionClass, T, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	}

	NewEntity->MissionDA = EntityToSpawn.MissionDA;
	NewEntity->FinishSpawning(T);
	ActiveEntities.Add(NewEntity);

	return NewEntity;
}

void AMV_Map::LoadSavedEntities(const TArray<FAMV_EntityBaseInfo>& EntitiesToSpawn)
{
	for (const FAMV_EntityBaseInfo& EntityToSpawn : EntitiesToSpawn)
	{
		SpawnEntity(EntityToSpawn);
	}
}

void AMV_Map::LoadSavedQuests(const TArray<FAMV_QuestInfo>& QuestGiversToSpawn)
{
	for (const FAMV_QuestInfo& QuestToSpawn : QuestGiversToSpawn)
	{
		SpawnQuestGiver(QuestToSpawn);
	}
}

bool AMV_Map::IsOverlappingAnyEntity(const FVector& Vector1, const int32 OverlapRadius) const
{
	for (const AMV_EntityBase* const &EntityBase : ActiveEntities)
	{
		const float Distance = FVector::Dist(Vector1, EntityBase->GetActorLocation());
		if (Distance < (OverlapRadius * 2))
		{
			return true;
		}
	}
	return false;
}

int32 AMV_Map::GetActiveEnemies() const
{
	int Amount = 0;

	for (const AMV_EntityBase* const &ActiveEntity : ActiveEntities)
	{
		if (ActiveEntity->MissionDA->MissionType == EMissionType::EnemyCamp || ActiveEntity->MissionDA->MissionType == EMissionType::Enemy)
		{
			++Amount;
		}
	}

	return Amount;
}

const int32 AMV_Map::GetSevenCharactersArea() const
{

	return 1;
}
