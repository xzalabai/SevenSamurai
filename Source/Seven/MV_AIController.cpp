#include "MV_AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "MVSevenCharacter.h"
#include "MV_Map.h"
#include "MV_Enemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MV_EntityBase.h"
#include "Kismet\GameplayStatics.h"

void AMV_AIController::BeginPlay()
{
	Super::BeginPlay();
	GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AMV_AIController::RequestFinished);
	
	Map = Cast<AMV_Map>(UGameplayStatics::GetActorOfClass(this, AMV_Map::StaticClass()));
	if (Map)
	{
		// Can be nullptr in case of VillageView
		Map->OnDayPeriodChange.AddUObject(this, &AMV_AIController::OnDayPeriodChange);
	}
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
	if (UBlackboardComponent* BlackBoardComponent = GetBlackboardComponent())
	{
		BlackBoardComponent->SetValueAsBool(TEXT("bCanMove"), bEnable);
	}
	StopMovement();
}

void AMV_AIController::RequestFinished(FAIRequestID AIRequestID, const FPathFollowingResult& PathFollowingResult)
{
	bMovementFinished = PathFollowingResult.IsSuccess();
}

void AMV_AIController::OnDayPeriodChange(EDayPart DayPart)
{
	if (UBlackboardComponent* BlackBoardComponent = GetBlackboardComponent())
	{
		UE_LOG(LogTemp, Display, TEXT("[AMV_AIController].OnDayPeriodChange CAMP!"));
		BlackBoardComponent->SetValueAsBool(TEXT("bCanCamp"), DayPart == EDayPart::Night ? true : false);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("[AMV_AIController].OnDayPeriodChange BlackBoard is yet NOT set. %s"), *GetPawn()->GetName());
	}
	
}

bool AMV_AIController::IsMovementFinished()
{
	return bMovementFinished;
}

void AMV_AIController::Camp(const bool bEnable) const
{
	AMV_Enemy* Enemy = Cast<AMV_Enemy>(GetPawn());
	Enemy->Camp(bEnable);
}



