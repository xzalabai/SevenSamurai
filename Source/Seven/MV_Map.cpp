#include "MV_Map.h"
#include "GameController.h"
#include "PaperSpriteActor.h"
#include "PaperSpriteComponent.h"
#include "MV_Enemy.h"
#include "MV_EntityBase.h"
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
			GenerateNewEnemy();
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

FVector AMV_Map::GetRandomPointOnMap() const
{
	const UPaperSpriteComponent* SpriteComponent = GetRenderComponent();
	FBoxSphereBounds SpriteBounds = SpriteComponent->CalcBounds(SpriteComponent->GetComponentTransform());
	FVector Center = SpriteBounds.Origin;
	FVector Extent = SpriteBounds.BoxExtent;
	FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(Center, Extent);
	RandomPoint.Z = Center.Z;

	return RandomPoint;
}

TObjectPtr<AMVSevenCharacter> AMV_Map::GetMVSevenCharacter() const
{
	return MVSevenCharacter;
}

void AMV_Map::GenerateNewEnemy()
{
	// Get random location
	FVector RandomPoint = GetRandomPointOnMap();
	RandomPoint.Z += 1;
	
	// Generate random enemy
	AvailableEnemies.Emplace(NewObject<UMissionDA>());
	const UMissionDA* EnemyTemplate = AvailableEnemies[0];
	UMissionDA* NewEnemy = AvailableEnemies[AvailableEnemies.Num() - 1];

	NewEnemy->Name = "RandomGuy";
	NewEnemy->Description = "Random Guy Description";
	NewEnemy->Image = EnemyTemplate->Image;
	NewEnemy->MissionCompleteImage = EnemyTemplate->MissionCompleteImage;
	NewEnemy->MissionType = EnemyTemplate->MissionType;
	NewEnemy->EnemiesToSpawn = EnemyTemplate->EnemiesToSpawn;
	NewEnemy->Reward = EnemyTemplate->Reward;
	NewEnemy->SpecialCharacter = nullptr;

	SpawnEntity(FAMV_EntityBaseInfo(RandomPoint, NewEnemy));
}

void AMV_Map::GenerateEntites()
{
	// Generate Villages
	for (int i = 0; i < 3; i++)
	{
		GenerateNewEnemy();
	}

	// Generate Enemies
	for (int i = 0; i < 2; i++)
	{
		FVector RandomPointOnMap = GetRandomPointOnMap();
		RandomPointOnMap.Z += 1;
		SpawnEntity(FAMV_EntityBaseInfo(RandomPointOnMap, AvailableVillages[i]));
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
