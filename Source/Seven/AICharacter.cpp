#include "AICharacter.h"
#include "SevenCharacter.h"
#include "SevenPlayerController.h"
#include "AnimationComponent.h"
#include "AIController.h"
#include <Kismet\GameplayStatics.h>

UAICharacter::UAICharacter()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAICharacter::BeginPlay()
{
	Super::BeginPlay();
}

void UAICharacter::MoveTo(bool bToSevenCharacter, bool bBlockingStance)
{
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(GetOwner());
	if (!SevenCharacter)
	{
		return;
	}

	if (bBlockingStance)
	{
		SevenCharacter->AC_Animation->Block(true);
	}
	else
	{
		SevenCharacter->AC_Animation->Guard(true);
	}

	AAIController* AIController = Cast<AAIController>(SevenCharacter->GetController());
	ASevenCharacter* EnemyToAttack = SelectEnemy();

	if (!AIController)
	{
		return;
	}

	AIController->SetFocus(EnemyToAttack);

	FVector FinalDestination;

	if (bToSevenCharacter)
	{
		if (EnemyToAttack)
		{
			FinalDestination = EnemyToAttack->GetActorLocation();
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[AEnemyCharacter].MoveTo Couldn't fetch SevenCharacter"));
		}
	}
	else
	{
		FinalDestination = GetRandomGuardPoint();
	}

	AIController->MoveToLocation(FinalDestination, 150.0f);
}	

const FVector UAICharacter::GetRandomGuardPoint()
{
	const FVector Right = (FMath::RandBool() ? GetOwner()->GetActorRightVector() : GetOwner()->GetActorRightVector() * (-1)) * 400;
	const FVector Backwards = GetOwner()->GetActorForwardVector() * (-1) * 400;

	const FVector FinalDestination = GetOwner()->GetActorLocation() + Right + Backwards;
	return FinalDestination;
}

ASevenCharacter* UAICharacter::SelectEnemy()
{
	ASevenPlayerController* SevenPlayerController = Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(SevenPlayerController->GetPossessedCharacter());

	return SevenCharacter;
}
