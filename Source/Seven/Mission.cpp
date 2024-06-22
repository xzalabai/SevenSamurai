#include "Mission.h"
#include "Components/SphereComponent.h"
#include <Kismet\GameplayStatics.h>
#include "SevenCharacter.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameController.h"
#include "AIController.h"


AMission::AMission()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	SetRootComponent(RootSceneComponent);

	Area = CreateDefaultSubobject<USphereComponent>(TEXT("Area"));
	SevenCharactersPosition = CreateDefaultSubobject<USceneComponent>(TEXT("SevenCharacterPosition"));
	
	for (int i = 0; i < 3; i++)
	{
		FString NameFString = "EnemySpawn" + FString::FromInt(i);
		FName Name = FName(*NameFString);
		EnemySpawns.Add(CreateDefaultSubobject<USceneComponent>(Name));
	}

	Area->SetupAttachment(RootComponent);
	SevenCharactersPosition->SetupAttachment(RootComponent);
}

void AMission::BeginPlay()
{
	Super::BeginPlay();
	Area->OnComponentBeginOverlap.AddDynamic(this, &AMission::OnOverlapBegin);
	check(MissionType != EMissionType::NotProvided);
	
	if (bSideMission)
	{
		// this will be turned on for all
		ActivateMission(false);
	}
	else
	{
		ActivateMission(true);
	}

	const UGameInstance* GameInstance = Cast<UGameInstance>(GetWorld()->GetGameInstance());
	UGameController* GameController = Cast<UGameController>(GameInstance->GetSubsystem<UGameController>());
	GameController->OnStatusUpdate.AddUObject(this, &AMission::OnStatusUpdate);
}

void AMission::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(OtherActor);
	if (SevenCharacter->IsEnemy() || !SevenCharacter->IsPlayerControlled())
	{
		return;
	}
	MissionStarted();
	MoveAlliesToPlace();
}

void AMission::MissionComplete(bool bWin) const
{
	if (!bSideMission && SideMission)
	{
		SideMission->ActivateMission(true);
	}
}

void AMission::ActivateMission(bool bEnable)
{
	UE_LOG(LogTemp, Display, TEXT("[AMission].ActivateMission %d"), ID);
	Area->SetGenerateOverlapEvents(bEnable);
}

void AMission::MissionStarted() const
{
	Area->SetGenerateOverlapEvents(false);

	for (const TPair<int32, TSubclassOf<AEnemyCharacter>>& Pair : EnemiesToSpawn)
	{
		for (int i = 0; i < Pair.Key; i++)
		{
			const FTransform T(EnemySpawns[i % EnemySpawns.Num()]->GetComponentRotation(), EnemySpawns[i % EnemySpawns.Num()]->GetComponentLocation());
			AEnemyCharacter* Enemy = GetWorld()->SpawnActorDeferred<AEnemyCharacter>(Pair.Value, T, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			Enemy->MissionType = MissionType;
			// Set AI
			Enemy->FinishSpawning(T);
		}
	}
}

void AMission::MoveAlliesToPlace()
{
	const UGameInstance* GameInstance = Cast<UGameInstance>(GetWorld()->GetGameInstance());
	UGameController* GameController = Cast<UGameController>(GameInstance->GetSubsystem<UGameController>());

	const TArray<const ASevenCharacter*>& AIControlledAllies = GameController->GetAIControlledAllies();

	for (const ASevenCharacter* AIAlly : AIControlledAllies)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UMissions].MoveSevenCharactersToPlace AIAlly move to mission area: %d"), AIAlly->GetUniqueID());
		AAIController* AIController = Cast<AAIController>(AIAlly->GetController());
		UBlackboardComponent* BlackBoardComponent = AIController->GetBlackboardComponent();
		BlackBoardComponent->SetValueAsBool(TEXT("bFollowPlayer"), false);
		BlackBoardComponent->SetValueAsObject(TEXT("PositionToGo"), SevenCharactersPosition);
	}
}

void AMission::OnStatusUpdate(const AActor* Actor, const EEnemyStatus Status)
{
	const ASevenCharacter* KilledCharacter = Cast<ASevenCharacter>(Actor);
	return;
	if (KilledCharacter->IsEnemy())
	{
		EnemyKilledCount++;
	}
	else
	{
		SevenCharactersKilledCount++;
	}

	if (SevenCharactersKilledCount == SevenCharacterCount)
	{
		UE_LOG(LogTemp, Display, TEXT("[UMissions].OnMissionEnd LOST"));

		OnMissionEnd.Broadcast(false);
		MissionComplete(false);

		const UGameInstance* GameInstance = Cast<UGameInstance>(GetWorld()->GetGameInstance());
		UGameController* GameController = Cast<UGameController>(GameInstance->GetSubsystem<UGameController>());

		if (GameController)
		{
			GameController->Restart();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[UMissions].OnMissionEnd GameController is NULLPPTR"));
		}

	}

	if (EnemyKilledCount == EnemiesToSpawn.Num())
	{
		UE_LOG(LogTemp, Display, TEXT("[UMissions].OnMissionEnd WIN"));

		OnMissionEnd.Broadcast(true);
		MissionComplete(true);
	}
}

