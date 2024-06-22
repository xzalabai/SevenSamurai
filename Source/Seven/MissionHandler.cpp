#include "MissionHandler.h"
#include "Mission.h"
#include "AICharacter.h"
#include "EnemyCharacter.h"
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
}