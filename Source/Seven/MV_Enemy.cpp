#include "MV_Enemy.h"
#include "MissionDA.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "MV_AIController.h"

AMV_Enemy::AMV_Enemy()
{
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement Component"));
}

void AMV_Enemy::BeginPlay()
{
	Super::BeginPlay();

	if (MissionDA->MissionType == EMissionType::EnemyCamp)
	{
		UpdateImage(EnemyCampImage);
	}
}

void AMV_Enemy::OnPossessed()
{
	if (MissionDA->bCompleted)
	{
		Controller = nullptr;
		return;
	}

	AAIController* AIController = Cast<AAIController>(GetController());
	UBlackboardComponent* BlackBoardComponent = AIController->GetBlackboardComponent();
	BlackBoardComponent->SetValueAsBool(TEXT("bCanMove"), MissionDA->MissionType == EMissionType::EnemyCamp ? false : true);
}

void AMV_Enemy::Camp(const bool bEnable)
{
	if (bEnable)
	{
		// Copy original Enemy behavior
		MissionDA->MissionType = EMissionType::EnemyCamp;
		UpdateImage(EnemyCampImage);
	}
	else
	{
		MissionDA->MissionType = EMissionType::Enemy;
		UpdateImage();
	}
	
	AMV_AIController* MV_AIController = Cast<AMV_AIController>(GetController());
	MV_AIController->EnableMoving(!bEnable);
}



