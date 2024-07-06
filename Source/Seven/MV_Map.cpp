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
		// FIRST RUN

		// Spawn 3 villages, on designated points

		for (int i = 0; i < 2; i++)
		{
			FVector Pos = GetRandomPointOnMap();
			FTransform T = FTransform{ FRotator(0, -180, -90), FVector(Pos.X, Pos.Y, Pos.Z + 1), FVector(1,1,1) };
			AMV_Village* NewMission = GetWorld()->SpawnActorDeferred<AMV_Village>(VillageClass, T, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			NewMission->MissionDA = AvailableVillages[i];
			NewMission->FinishSpawning(T);
			ActiveEntities.Add(NewMission);
		}
	}
	else
	{
		for (const FAMV_EntityBaseInfo& EntityToSpawn : EntitiesToSpawn)
		{
			FTransform T{ FRotator(0, -180, -90), EntityToSpawn.Position, FVector(1,1,1) };

			if (EntityToSpawn.MissionDA->MissionType == EMissionType::LiberatePlace)
			{
				AMV_Village* NewMission = GetWorld()->SpawnActorDeferred<AMV_Village>(VillageClass, T, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
				NewMission->MissionDA = EntityToSpawn.MissionDA;
				NewMission->FinishSpawning(T);
			}
			else if (EntityToSpawn.MissionDA->MissionType == EMissionType::EnemyCamp || EntityToSpawn.MissionDA->MissionType == EMissionType::Enemy)
			{
				AMV_Enemy* Enemy = GetWorld()->SpawnActorDeferred<AMV_Enemy>(EnemyClass, T, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
				Enemy->MissionDA = EntityToSpawn.MissionDA;
				Enemy->FinishSpawning(T);
			}
			else
			{
				AMV_EntityBase* NewMission = GetWorld()->SpawnActorDeferred<AMV_EntityBase>(MissionClass, T, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
				NewMission->MissionDA = EntityToSpawn.MissionDA;
				NewMission->FinishSpawning(T);
			}
		}

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
			SpawnEnemy();
			break;
		case 10:
			Time.DayPart = EDayPart::Day;
			OnDayPeriodChange.Broadcast(Time.DayPart);
			break;
		case 17:
			Time.DayPart = EDayPart::Evening;
			OnDayPeriodChange.Broadcast(Time.DayPart);
			SpawnEnemy();
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

void AMV_Map::SpawnEnemy()
{
	const FVector RandomPoint = GetRandomPointOnMap();
	const FTransform T = FTransform{ FRotator(0, -180, -90), FVector(RandomPoint.X, RandomPoint.Y, RandomPoint.Z + 1), FVector(1.5, 1.5, 1.5) };
	AMV_Enemy* Enemy = GetWorld()->SpawnActorDeferred<AMV_Enemy>(
		EnemyClass,
		T,
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
	);

	AvailableEnemies.Emplace(NewObject<UMissionDA>());
	UMissionDA* EnemyTemplate = AvailableEnemies[0];
	UMissionDA* NewEnemy = AvailableEnemies[AvailableEnemies.Num() - 1];
	
	NewEnemy->Name = "RandomGuy";
	NewEnemy->Description = "Random Guy Description";
	NewEnemy->Image = EnemyTemplate->Image;
	NewEnemy->MissionCompleteImage = EnemyTemplate->MissionCompleteImage;
	NewEnemy->MissionType = EnemyTemplate->MissionType;
	NewEnemy->EnemiesToSpawn = EnemyTemplate->EnemiesToSpawn;
	NewEnemy->Reward = EnemyTemplate->Reward;
	NewEnemy->SpecialCharacter = nullptr;

	Enemy->MissionDA = NewEnemy;
	Enemy->FinishSpawning(T);

	ActiveEntities.Add(Enemy);
}
