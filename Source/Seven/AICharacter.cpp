#include "AICharacter.h"
#include "SevenCharacter.h"
#include "AttackComponent.h"
#include "GameController.h"
#include "AnimationComponent.h"
#include "AIController.h"
#include "Kismet\GameplayStatics.h"
#include "Kismet\KismetMathLibrary.h"
#include "EnemyCharacter.h"

UAICharacter::UAICharacter()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAICharacter::BeginPlay()
{
	Super::BeginPlay();

	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(GetOwner());
	AAIController* AIController = Cast<AAIController>(SevenCharacter->GetController());

	check(AIController);

	AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &UAICharacter::RequestFinished);
	
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
	AIController->SetMoveBlockDetection(false);
	//DrawDebugSphere(GetWorld(), FinalDestination, 80, 12, FColor::Black, true, -1);
	AIController->MoveToLocation(FinalDestination, 80.0f);
}

void UAICharacter::MoveTo(const FVector& Position)
{
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(GetOwner());
	AAIController* AIController = Cast<AAIController>(SevenCharacter->GetController());
	AIController->MoveToLocation(Position, 80.0f);
}

const FVector UAICharacter::GetRandomGuardPoint()
{
	const FVector Right = (FMath::RandBool() ? GetOwner()->GetActorRightVector() : GetOwner()->GetActorRightVector() * (-1)) * 300;
	const FVector Backwards = GetOwner()->GetActorForwardVector() * (-1) * 300;

	const FVector FinalDestination = GetOwner()->GetActorLocation() + Right + Backwards;
	return FinalDestination;
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
		UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter]Fire.TargetedEnemy %s"), *TargetedEnemy->GetName());
		//MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform("MW_LightAttackAttacker", TargetedEnemy->VictimDesiredPosition->GetComponentTransform());

		// Rotate character towards enemy
		FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(Bot->GetActorLocation(), TargetedEnemy->GetActorLocation());
		Bot->RootComponent->SetWorldRotation(PlayerRot);

		if (!Bot->AC_AttackComponent->LightAttack(TargetedEnemy))
		{
			Bot->AttackEnd();
		}
	}

	if (AEnemyCharacter* BotEnemyCharacter = Cast<AEnemyCharacter>(GetOwner()))
	{
		BotEnemyCharacter->ReturnAttackToken();
	}
}

void UAICharacter::FollowSevenCharacter(const ASevenCharacter* SevenCharacter)
{
	MoveTo(true, false);
}

ASevenCharacter* UAICharacter::SelectEnemy()
{
	ASevenCharacter* Bot = Cast<ASevenCharacter>(GetOwner());
	
	const UGameInstance* GameInstance = Cast<UGameInstance>(GetWorld()->GetGameInstance());
	UGameController* GameController = Cast<UGameController>(GameInstance->GetSubsystem<UGameController>());

	if (Bot && Bot->IsEnemy())
	{
		// Find SevenCharacter

		ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(GameController->GetPossessedCharacter());
		return SevenCharacter;
	}
	else
	{
		// Find EnemyCharacter
		ASevenCharacter* SevenCharacter = GameController->GetAnyAliveEnemy();
		return SevenCharacter;
	}
	UE_LOG(LogTemp, Error, TEXT("[AEnemyCharacter]SelectEnemy couldn't Select Enemy"));
	return nullptr;
}
