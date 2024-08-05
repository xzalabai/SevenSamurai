#include "AICharacter.h"
#include "SevenCharacter.h"
#include "AttackComponent.h"
#include "SevenGameMode.h"
#include "AnimationComponent.h"
#include "AIController.h"
#include "Kismet\GameplayStatics.h"
#include "Kismet\KismetMathLibrary.h"
#include "EnemyCharacter.h"

UAICharacter::UAICharacter()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAICharacter::BeginPlay()
{
	Super::BeginPlay();

	const ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(GetOwner());
	const AAIController* AIController = Cast<AAIController>(SevenCharacter->GetController());
	AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &UAICharacter::RequestFinished);
}

void UAICharacter::MoveTo(const FVector& Position)
{
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(GetOwner());
	AAIController* AIController = Cast<AAIController>(SevenCharacter->GetController());
	AIController->MoveToLocation(Position, 80.0f);
}

void UAICharacter::RequestFinished(FAIRequestID x, const FPathFollowingResult& xx)
{
	//UE_LOG(LogTemp, Error, TEXT("[AEnemyCharacter].FINISHED %d"), xx.IsSuccess() ? 1 : 0);
	bMovementFinished = xx.IsSuccess();
}

bool UAICharacter::IsMovementFinished()
{
	return bMovementFinished;
}

void UAICharacter::ResetMovementFinished()
{
	bMovementFinished = false;
}

void UAICharacter::FollowSevenCharacter(const ASevenCharacter* SevenCharacter)
{
	//MoveTo(true, false);
}
