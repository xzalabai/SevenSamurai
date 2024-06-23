#include "MV_AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"


void AMV_AIController::BeginPlay()
{
	Super::BeginPlay();
	GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AMV_AIController::RequestFinished);
}

void AMV_AIController::MoveCharacterTo(const FVector& Position)
{
	AActor* ControlledActor = GetPawn();
	//UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, FVector(Position.X, Position.Y, ControlledActor->GetActorLocation().Z));
	UE_LOG(LogTemp, Display, TEXT("[AMV_AIController].MoveCharacterTo"));
	MoveToLocation(FVector(Position.X, Position.Y, ControlledActor->GetActorLocation().Z), 10.0f);
}

void AMV_AIController::RequestFinished(FAIRequestID AIRequestID, const FPathFollowingResult& PathFollowingResult)
{
	bMovementFinished = PathFollowingResult.IsSuccess();
}


