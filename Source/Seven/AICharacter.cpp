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

void UAICharacter::MoveTo(bool bToSevenCharacter, bool bBlockingStance)
{
	const ASevenCharacter* const SevenCharacter = Cast<ASevenCharacter>(GetOwner());

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

	if (!AIController || !EnemyToAttack)
	{
		return;
	}

	FVector FinalDestination = bToSevenCharacter ? EnemyToAttack->GetActorLocation() : GetRandomGuardPointAroundEnemy(EnemyToAttack);
	AIController->SetFocus(EnemyToAttack);
	AIController->SetMoveBlockDetection(false);
	AIController->MoveToLocation(FinalDestination, 80.0f);
	//DrawDebugSphere(GetWorld(), FinalDestination, 80, 12, FColor::Black, true, -1);
}

void UAICharacter::MoveTo(const FVector& Position)
{
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(GetOwner());
	AAIController* AIController = Cast<AAIController>(SevenCharacter->GetController());
	AIController->MoveToLocation(Position, 80.0f);
}

const FVector UAICharacter::GetRandomGuardPointAroundEnemy(const ASevenCharacter* const Enemy)
{
	const FVector EnemyPosition = Enemy->GetActorLocation();
	const FVector EnemyForwardVector = Enemy->GetActorForwardVector();
	
	const int Left = FMath::RandBool() ? (-1) : 1;
	const int RandomOffsetX = FMath::RandRange(500, 1000);
	const int RandomOffsetY = FMath::RandRange(500, 2000) * Left;

	const FVector EnemyForwardVectorOffset = FVector(EnemyForwardVector.X * RandomOffsetX, EnemyForwardVector.Y * RandomOffsetY, EnemyForwardVector.Z);

	const FVector FinalDestination = FVector(EnemyPosition.X + EnemyForwardVectorOffset.X, EnemyPosition.Y + EnemyForwardVectorOffset.Y, EnemyPosition.Z);

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
	AEnemyCharacter* EnemyCharacter = Cast<AEnemyCharacter>(GetOwner());
	ASevenCharacter* TargetedEnemy = EnemyCharacter->GetClosestEnemyInRange();
	if (TargetedEnemy)
	{
		UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter]Fire.TargetedEnemy %s"), *TargetedEnemy->GetName());
		//MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform("MW_LightAttackAttacker", TargetedEnemy->VictimDesiredPosition->GetComponentTransform());

		// Rotate character towards enemy
		FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(EnemyCharacter->GetActorLocation(), TargetedEnemy->GetActorLocation());
		EnemyCharacter->RootComponent->SetWorldRotation(PlayerRot);

		if (!EnemyCharacter->AC_AttackComponent->LightAttack(TargetedEnemy))
		{
			EnemyCharacter->AttackEnd();
		}
	}
	else
	{
		EnemyCharacter->AttackEnd();
	}

	EnemyCharacter->ReturnAttackToken();
}

void UAICharacter::FollowSevenCharacter(const ASevenCharacter* SevenCharacter)
{
	MoveTo(true, false);
}

ASevenCharacter* UAICharacter::SelectEnemy()
{
	ASevenCharacter* Bot = Cast<ASevenCharacter>(GetOwner());
	ASevenGameMode* SevenGameMode = Cast<ASevenGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	if (Bot && Bot->IsEnemy())
	{
		// Find SevenCharacter

		ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(SevenGameMode->GetPossessedCharacter());
		return SevenCharacter;
	}
	else
	{
		// Find EnemyCharacter
		ASevenCharacter* SevenCharacter = SevenGameMode->GetAnyAliveEnemy();
		return SevenCharacter;
	}
	UE_LOG(LogTemp, Error, TEXT("[AEnemyCharacter]SelectEnemy couldn't Select Enemy"));
	return nullptr;
}
