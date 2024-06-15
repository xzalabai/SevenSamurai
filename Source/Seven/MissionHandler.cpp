#include "MissionHandler.h"
#include "Mission.h"
#include "AICharacter.h"
#include "EnemyCharacter.h"
#include "SevenPlayerController.h"
#include "GameController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet\GameplayStatics.h"
#include "AIController.h"

AMissionHandler::AMissionHandler()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AMissionHandler::BeginPlay()
{
	Super::BeginPlay();
	ActiveMissionID = -1;
	StoreMissions();
	check(EnemyClassToSpawn);

	TObjectPtr<ASevenPlayerController> SevenPlayerController = Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	SevenPlayerController->OnUpdateStatus.AddUObject(this, &AMissionHandler::OnStatusUpdate);
}

void AMissionHandler::StoreMissions()
{
	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, AMission::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (AMission* Mission = Cast<AMission>(Actor))
		{
			if (Missions.Contains(Mission->ID))
			{
				UE_LOG(LogTemp, Error, TEXT("[UMissions].StoreMissions Already contains same ID"));
				continue;
			}
			
			Missions.Add(Mission->ID, Mission);
			Mission->OnCharacterOverlappedMission.BindUObject(this, &AMissionHandler::MissionStarted);
		}
	}
}

void AMissionHandler::MissionStarted(uint32 ID)
{
	UE_LOG(LogTemp, Warning, TEXT("[UMissions].MissionStarted Mission ID %d has started"), ID);

	const AMission* const ActiveMission = Missions[ID];
	ActiveMissionID = ID;
	ActiveMission->MissionStarted();

	EnemyKilledCount = 0;
	SevenCharactersKilledCount = 0;
	MoveAlliesToPlace();
}

void AMissionHandler::MovingToMissionArea()
{
	ASevenPlayerController* PlayerController = Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	TArray<const ASevenCharacter*> AIControlledAllies = PlayerController->GetAIControlledAllies();

	for (const ASevenCharacter* AIAlly : AIControlledAllies)
	{
		AAIController* AIController = Cast<AAIController>(AIAlly->GetController());
		UBlackboardComponent* BlackBoardComponent = AIController->GetBlackboardComponent();
		BlackBoardComponent->SetValueAsBool(TEXT("bFollowPlayer"), true);
		BlackBoardComponent->SetValueAsObject(TEXT("PositionToGo"), PlayerController->GetPossessedCharacter());
	}
}

void AMissionHandler::MoveAlliesToPlace()
{
	ASevenPlayerController* PlayerController = Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (!PlayerController || ActiveMissionID == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UMissions].MoveSevenCharactersToPlace PlayerController or wrong ActiveMissionID"));
		return;
	}

	const AMission* ActiveMission = Missions[ActiveMissionID];

	TArray<const ASevenCharacter*> AIControlledAllies = PlayerController->GetAIControlledAllies();

	for (const ASevenCharacter* AIAlly : AIControlledAllies)
	{
		AAIController* AIController = Cast<AAIController>(AIAlly->GetController());
		UBlackboardComponent* BlackBoardComponent = AIController->GetBlackboardComponent();
		BlackBoardComponent->SetValueAsBool(TEXT("bFollowPlayer"), false);
		BlackBoardComponent->SetValueAsObject(TEXT("PositionToGo"), ActiveMission->SevenCharactersPosition);
	}
}

void AMissionHandler::OnStatusUpdate(const AActor* Actor, const EEnemyStatus Status)
{
	if (ActiveMissionID == -1 || Status != EEnemyStatus::Dead)
	{
		// No mission active
		return;
	}

	const AMission* ActiveMission = Missions[ActiveMissionID];
	const ASevenCharacter* KilledCharacter = Cast<ASevenCharacter>(Actor);

	if (KilledCharacter->IsEnemy())
	{
		EnemyKilledCount++;
	}
	else
	{
		SevenCharactersKilledCount++;
	}

	if (SevenCharactersKilledCount == ActiveMission->SevenCharacterCount)
	{
		UE_LOG(LogTemp, Display, TEXT("[UMissions].OnMissionEnd LOST"));

		OnMissionEnd.Broadcast(false);
		ActiveMission->MissionComplete(false);

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

	if (EnemyKilledCount == ActiveMission->EnemiesToSpawn.Num())
	{
		UE_LOG(LogTemp, Display, TEXT("[UMissions].OnMissionEnd WIN"));
		
		OnMissionEnd.Broadcast(true);
		ActiveMission->MissionComplete(true);
	}
}
