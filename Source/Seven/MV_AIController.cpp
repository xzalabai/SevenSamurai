#include "MV_AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "MVSevenCharacter.h"
#include "MV_Map.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MV_EntityBase.h"
#include <Kismet\GameplayStatics.h>

void AMV_AIController::BeginPlay()
{
	Super::BeginPlay();
	GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AMV_AIController::RequestFinished);
	
	Map = Cast<AMV_Map>(UGameplayStatics::GetActorOfClass(this, AMV_Map::StaticClass()));
	Map->OnDayPeriodChange.AddUObject(this, &AMV_AIController::OnDayPeriodChange);

}

void AMV_AIController::MoveCharacterTo(const FVector& Position)
{
	UE_LOG(LogTemp, Display, TEXT("[AMV_AIController].MoveCharacterTo"));

	AActor* ControlledActor = GetPawn();
	//UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, FVector(Position.X, Position.Y, ControlledActor->GetActorLocation().Z));
	MoveToLocation(FVector(Position.X, Position.Y, ControlledActor->GetActorLocation().Z), 10.0f);
}

void AMV_AIController::MoveToRandomPosition()
{
	UE_LOG(LogTemp, Display, TEXT("[AMV_AIController].MoveToRandomPosition"));
	const AMV_EntityBase* Entity = Cast<AMV_EntityBase>(GetPawn());
	FVector Position = Map->GetRandomPointOnMap();
	MoveCharacterTo(Map->GetRandomPointOnMap());
	//UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, FVector(Position.X, Position.Y, GetPawn()->GetActorLocation().Z));
}

void AMV_AIController::MoveToSevenCharacter()
{
	TObjectPtr<AMVSevenCharacter> MVSevenCharacter = Map->GetMVSevenCharacter();
	//UBlackboardComponent* BlackBoardComponent = GetBlackboardComponent();
	//BlackBoardComponent->SetValueAsBool(TEXT("bChaseSevenCharacter"), true);	

	MoveToActor(MVSevenCharacter.Get());
}

void AMV_AIController::EnableMoving(const bool bEnable)
{
	UBlackboardComponent* BlackBoardComponent = GetBlackboardComponent();
	BlackBoardComponent->SetValueAsBool(TEXT("bCanMove"), bEnable);
	StopMovement();
}

void AMV_AIController::RequestFinished(FAIRequestID AIRequestID, const FPathFollowingResult& PathFollowingResult)
{
	bMovementFinished = PathFollowingResult.IsSuccess();
}

void AMV_AIController::OnDayPeriodChange(EDayPart DayPart)
{
	UBlackboardComponent* BlackBoardComponent = GetBlackboardComponent();
	//BlackBoardComponent->SetValueAsBool(TEXT("bNight"), DayPart == EDayPart::Night ? true : false);
}

bool AMV_AIController::IsMovementFinished()
{
	return bMovementFinished;
}



