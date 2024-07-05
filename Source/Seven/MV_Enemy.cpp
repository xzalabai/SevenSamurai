#include "MV_Enemy.h"
#include "MissionDA.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "MV_AIController.h"

AMV_Enemy::AMV_Enemy()
{
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement Component"));
}

void AMV_Enemy::OnPossessed()
{
	UE_LOG(LogTemp, Display, TEXT("[AMV_Enemy] OnPossessed"));
	AAIController* AIController = Cast<AAIController>(GetController());
	UBlackboardComponent* BlackBoardComponent = AIController->GetBlackboardComponent();
	BlackBoardComponent->SetValueAsBool(TEXT("bCanMove"), true);
}

void AMV_Enemy::BeginPlay()
{
	Super::BeginPlay();
	if (MissionDA->bCompleted)
	{
		// Unplug Enemy from AI if he lost the battle
		Controller = nullptr;
	}
}

void AMV_Enemy::Camp(const bool bEnable)
{
	if (bEnable)
	{
		UpdateImage(EnemyCampImage);
		MissionDA->MissionType = EMissionType::BanditCamp;
	}
	else
	{
		UpdateImage(MissionDA->Image);
		MissionDA->MissionType = EMissionType::Enemy;
	}

	AMV_AIController* MV_AIController = Cast<AMV_AIController>(GetController());
	MV_AIController->EnableMoving(false);
}



