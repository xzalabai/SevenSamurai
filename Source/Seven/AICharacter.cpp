#include "AICharacter.h"
#include "SevenCharacter.h"
#include "SevenPlayerController.h"
#include "AttackComponent.h"
#include "AnimationComponent.h"
#include "AIController.h"
#include "Kismet\GameplayStatics.h"
#include "Kismet\KismetMathLibrary.h"

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

void UAICharacter::Fire()
{
	//Attack
	ASevenCharacter* Bot = Cast<ASevenCharacter>(GetOwner());

	if (!Bot)
	{
		return;
	}

	ASevenCharacter* TargetedEnemy = Bot->GetClosestEnemyInRange();
	Bot->AC_AttackComponent->LightAttack(TargetedEnemy);

	if (TargetedEnemy)
	{
		UE_LOG(LogTemp, Error, TEXT("[AEnemyCharacter]Fire.TargetedEnemy %s"), *TargetedEnemy->GetName());
		//MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform("MW_LightAttackAttacker", TargetedEnemy->VictimDesiredPosition->GetComponentTransform());

		// Rotate character towards enemy
		FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(Bot->GetActorLocation(), TargetedEnemy->GetActorLocation());
		Bot->RootComponent->SetWorldRotation(PlayerRot);

		if (!Bot->AC_AttackComponent->LightAttack(TargetedEnemy))
		{
			Bot->AttackEnd();
		}
	}
	Bot->ReturnAttackToken();
}

ASevenCharacter* UAICharacter::SelectEnemy()
{
	ASevenPlayerController* SevenPlayerController = Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(SevenPlayerController->GetPossessedCharacter());

	return SevenCharacter;
}
