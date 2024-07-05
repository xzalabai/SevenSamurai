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


void AMV_Map::BeginPlay()
{
	Super::BeginPlay();

	PrimaryActorTick.bCanEverTick = true;
	SetActorTickInterval(1.0f);

	const UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	const TArray<FAMV_EntityBaseInfo>& EntitiesToSpawn = GameController->RetrieveActiveEntities();
	MVSevenCharacter = Cast<AMVSevenCharacter>(UGameplayStatics::GetActorOfClass(this, AMVSevenCharacter::StaticClass()));

	if (EntitiesToSpawn.Num() == 0)
	{
		// First run

		// RESET EVERYTHING
		// TODO: do it better!
		for (UMissionDA* MissionDA : AvailableMissions)
		{
			MissionDA->bCompleted = false;
			MissionDA->bStarted = false;
		}

		// Spawn Village
		FVector Pos = GetRandomPointOnMap();
		DrawDebugPoint(GetWorld(), Pos, 10.0f, FColor::Red, false, 5.0f);
		FTransform T{ FRotator(0, -180, -90), FVector(Pos.X, Pos.Y, Pos.Z + 1), FVector(1,1,1) };
		AMV_Village* Village = GetWorld()->SpawnActorDeferred<AMV_Village>(VillageClass, T, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		Village->MissionDA = AvailableMissions[0];
		Village->FinishSpawning(T);
		ActiveEntities.Add(Village);
		//Villages.Add(Village);

		// Spawn Enemy
		Pos = GetRandomPointOnMap();
		DrawDebugPoint(GetWorld(), Pos, 10.0f, FColor::Red, false, 5.0f);
		T = FTransform{ FRotator(0, -180, -90), FVector(Pos.X, Pos.Y, Pos.Z + 1), FVector(1,1,1) };
		AMV_Enemy* Enemy = GetWorld()->SpawnActorDeferred<AMV_Enemy>(EnemyClass, T, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		Enemy->MissionDA = AvailableMissions[1];
		Enemy->FinishSpawning(T);
		ActiveEntities.Add(Enemy);
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
			else if (EntityToSpawn.MissionDA->MissionType == EMissionType::BanditCamp)
			{
				AMV_EntityBase* NewMission = GetWorld()->SpawnActorDeferred<AMV_EntityBase>(MissionClass, T, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
				NewMission->MissionDA = EntityToSpawn.MissionDA;
				NewMission->FinishSpawning(T);
			}
			else if (EntityToSpawn.MissionDA->MissionType == EMissionType::Enemy)
			{
				AMV_Enemy* Enemy = GetWorld()->SpawnActorDeferred<AMV_Enemy>(EnemyClass, T, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
				Enemy->MissionDA = EntityToSpawn.MissionDA;
				Enemy->FinishSpawning(T);
			}
		}

	}

	
}

void AMV_Map::Tick(float DeltaTime)
{
	
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