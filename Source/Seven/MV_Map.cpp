#include "MV_Map.h"
#include "GameController.h"
#include "PaperSpriteActor.h"
#include "PaperSpriteComponent.h"
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

	for (int i = 0; i < 3; i++)
	{
		//FName Name = CustomMath::ConcatFNameAndInt(FName("Area"), i);
		FName Name = TEXT("AREA") + i;
		UBoxComponent* BoxComponent = CreateDefaultSubobject<UBoxComponent>(Name);
		BoxComponent->SetupAttachment(RootComponent);
		Areas.Add(BoxComponent);
	}
}

void AMV_Map::BeginPlay()
{
	Super::BeginPlay();

	const UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	const TArray<FAMV_EntityBaseInfo>& EntitiesToSpawn = GameController->RetrieveActiveEntities();
	MVSevenCharacter = Cast<AMVSevenCharacter>(UGameplayStatics::GetActorOfClass(this, AMVSevenCharacter::StaticClass()));

	if (EntitiesToSpawn.Num() == 0)
	{
		GenerateEntites();
	}
	else
	{
		LoadStoredEntities(EntitiesToSpawn);
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

FVector AMV_Map::GetRandomPointOnMap(const UBoxComponent* const Area, const bool bShift, const int32 OverlapRadius) const
{
	bool bCanOverlap = (OverlapRadius == -1);
	FVector RandomPoint;
	const FBoxSphereBounds SpriteBounds = GetRenderComponent()->CalcBounds(GetRenderComponent()->GetComponentTransform());
	const FVector Origin = Area ? Area->Bounds.Origin : SpriteBounds.Origin;
	const FVector Extent = Area ? Area->Bounds.BoxExtent : SpriteBounds.BoxExtent;

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

TObjectPtr<AMVSevenCharacter> AMV_Map::GetMVSevenCharacter() const
{
	return MVSevenCharacter;
}

void AMV_Map::GenerateEntity(const int8 Index, EMissionType MissionType)
{
	FVector RandomPoint = GetRandomPointOnMap((Index >= 0 ? Areas[Index] : nullptr), true, 100);
	
	UMissionDA* NewEnemyMission = NewObject<UMissionDA>();

	if (MissionType == EMissionType::NotProvided)
	{
		uint32 RandomMissionIndex = FMath::RandRange(0, AvailableMissions.Num() - 1);
		NewEnemyMission = AvailableMissions[RandomMissionIndex];
	}
	else if (MissionType == EMissionType::Enemy)
	{
		const UMissionDA* EnemyTemplate = AvailableEnemies[0];

		NewEnemyMission->Name = "RandomGuy";
		NewEnemyMission->Description = "Random Guy Description";
		NewEnemyMission->Image = EnemyTemplate->Image;
		NewEnemyMission->MissionCompleteImage = EnemyTemplate->MissionCompleteImage;
		NewEnemyMission->MissionType = EnemyTemplate->MissionType;
		NewEnemyMission->EnemiesToSpawn = EnemyTemplate->EnemiesToSpawn;
		NewEnemyMission->Reward = EnemyTemplate->Reward;
		NewEnemyMission->bStarted = false;
		NewEnemyMission->bCompleted = false;
		NewEnemyMission->AreaIndex = Index;
		NewEnemyMission->SpecialCharacter = nullptr;

	}
	else if (MissionType == EMissionType::LiberatePlace)
	{
		uint32 RandomMissionIndex = FMath::RandRange(0, AvailableVillages.Num() - 1);
		const UMissionDA* VillageTemplate = AvailableVillages[RandomMissionIndex];

		NewEnemyMission->Name = VillageTemplate->Name;
		NewEnemyMission->Description = VillageTemplate->Description;
		NewEnemyMission->Image = VillageTemplate->Image;
		NewEnemyMission->MissionCompleteImage = VillageTemplate->MissionCompleteImage;
		NewEnemyMission->MissionType = VillageTemplate->MissionType;
		NewEnemyMission->EnemiesToSpawn = VillageTemplate->EnemiesToSpawn;
		NewEnemyMission->Reward = VillageTemplate->Reward;
		NewEnemyMission->bStarted = false;
		NewEnemyMission->bCompleted = false;
		NewEnemyMission->AreaIndex = Index;
		NewEnemyMission->SpecialCharacter = nullptr;
	}

	GeneratedMissions.Add(NewEnemyMission);
	SpawnEntity(FAMV_EntityBaseInfo(RandomPoint, NewEnemyMission));
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
			GenerateEntity(i);
		}
	}
}

void AMV_Map::SpawnEntity(const FAMV_EntityBaseInfo& EntityToSpawn)
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
}

void AMV_Map::LoadStoredEntities(const TArray<FAMV_EntityBaseInfo>& EntitiesToSpawn)
{
	for (const FAMV_EntityBaseInfo& EntityToSpawn : EntitiesToSpawn)
	{
		SpawnEntity(EntityToSpawn);
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
