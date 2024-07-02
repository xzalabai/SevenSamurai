#include "MV_Enemy.h"
#include "MissionDA.h"
#include "GameFramework/FloatingPawnMovement.h"

AMV_Enemy::AMV_Enemy()
{
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement Component"));
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
