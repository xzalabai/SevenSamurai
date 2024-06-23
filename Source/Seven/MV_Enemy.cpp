#include "MV_Enemy.h"
#include "GameFramework/FloatingPawnMovement.h"

AMV_Enemy::AMV_Enemy()
{
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement Component"));
}
