#include "Mission.h"
#include "Components/SphereComponent.h"
#include <Kismet\GameplayStatics.h>
#include "SevenCharacter.h"
#include "EnemyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SevenGameMode.h"
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

	ASevenGameMode* SevenGameMode = Cast<ASevenGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	
	SevenGameMode->UpdateMissionParameters(this);
	SevenGameMode->OnStatusUpdate.AddUObject(this, &AMission::OnStatusUpdate);

	Area->OnComponentBeginOverlap.AddDynamic(this, &AMission::OnOverlapBegin);

	ActivateMission(true);
	MissionStarted();
}

void AMission::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(OtherActor);
	if (SevenCharacter->IsEnemy() || !SevenCharacter->IsPlayerControlled())
	{
		return;
	}
	MissionStarted();
}

void AMission::ActivateMission(bool bEnable)
{
	UE_LOG(LogTemp, Display, TEXT("[AMission].ActivateMission %d"), ID);
	Area->SetGenerateOverlapEvents(bEnable);
}

void AMission::MissionStarted()
{
	Area->SetGenerateOverlapEvents(false);

	const UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	SevenCharacterCount = GameController->SelectedCharacters.Num();

	for (const TPair<TSubclassOf<AEnemyCharacter>, int32>& Pair : MissionDA->EnemiesToSpawn)
	{
		for (int i = 0; i < Pair.Value; i++)
		{
			const FTransform T(EnemySpawns[i % EnemySpawns.Num()]->GetComponentRotation(), EnemySpawns[i % EnemySpawns.Num()]->GetComponentLocation());
			AEnemyCharacter* Enemy = GetWorld()->SpawnActorDeferred<AEnemyCharacter>(Pair.Key, T, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			Enemy->MissionType = MissionDA->MissionType;
			Enemy->Difficulty = MissionDA->Difficulty;
			// Set AI
			Enemy->FinishSpawning(T);
			++EnemyCount;
			UE_LOG(LogTemp, Display, TEXT("[AMission] Spawn Enemy: %d, %d"), Pair.Key, i);
		}
	}
	UE_LOG(LogTemp, Display, TEXT("[AMission].MissionStarted: %d, SevenCharactersCount: %d, EnemyCount: %d"), ID, SevenCharacterCount, EnemyCount);

	MoveAlliesToPlace();
}

void AMission::MoveAlliesToPlace()
{
	ASevenGameMode* SevenGameMode = Cast<ASevenGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	const TArray<const ASevenCharacter*>& AIControlledAllies = SevenGameMode->GetAIControlledAllies();

	for (const ASevenCharacter* AIAlly : AIControlledAllies)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UMissions].MoveSevenCharactersToPlace AIAlly move to mission area: %d"), AIAlly->GetUniqueID());
		AAIController* AIController = Cast<AAIController>(AIAlly->GetController());
		UBlackboardComponent* BlackBoardComponent = AIController->GetBlackboardComponent();
		BlackBoardComponent->SetValueAsBool(TEXT("bFollowPlayer"), false);
		BlackBoardComponent->SetValueAsObject(TEXT("PositionToGo"), SevenCharactersPosition);
	}
}

void AMission::OnStatusUpdate(const ASevenCharacter* SevenCharacter, const ECharacterState Status)
{
	if (Status != ECharacterState::Dead)
	{
		return;
	}
		
	ASevenGameMode* SevenGameMode = Cast<ASevenGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (SevenCharacter->IsEnemy())
	{
		++EnemyKilledCount;

		if (EnemyKilledCount == EnemyCount)
		{
			UE_LOG(LogTemp, Display, TEXT("[UMissions].OnMissionEnd WIN"));
			SevenGameMode->MissionEnd(true);
		}

	}
	else
	{
		++SevenCharactersKilledCount;

		if (SevenCharactersKilledCount == SevenCharacterCount)
		{
			UE_LOG(LogTemp, Display, TEXT("[UMissions].OnMissionEnd LOST"));
			SevenGameMode->MissionEnd(false);
		}
	}
}

