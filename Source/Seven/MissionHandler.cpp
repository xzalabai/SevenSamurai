#include "MissionHandler.h"
#include "Mission.h"
#include "AICharacter.h"
#include "EnemyCharacter.h"
#include "SevenPlayerController.h"
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

	const AMission* ActiveMission = Missions[ID];
	ActiveMissionID = ID;
	ActiveMission->Area->SetGenerateOverlapEvents(false);

	FActorSpawnParameters SpawnParams;
	AEnemyCharacter* NewEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(EnemyClassToSpawn, ActiveMission->EnemySpawn->GetComponentLocation(), FRotator(), SpawnParams);
	
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
