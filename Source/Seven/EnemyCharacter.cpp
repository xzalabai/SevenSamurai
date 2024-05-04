#include "EnemyCharacter.h"
#include "Weapon.h"
#include "AnimationComponent.h"
#include "SevenPlayerController.h"
#include "ComboManager.h"
#include "Particles/ParticleSystem.h"
#include "AttackComponent.h"
#include "Kismet\GameplayStatics.h"
#include "Kismet\KismetMathLibrary.h"
#include <AIController.h>

AEnemyCharacter::AEnemyCharacter()
{
	IncomingAttackParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("ImpactParticles"));
}

void AEnemyCharacter::InitiateAttack()
{
	Fire(FInputActionValue());
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	SevenPlayerController = Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	SevenPlayerController->UpdateStatus(this);
}

void AEnemyCharacter::IncomingAttack()
{
	if (SevenPlayerController)
	{
		SevenPlayerController->UpdateStatus(this, EEnemyStatus::IncomingAttack);

		if (ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(SevenPlayerController->GetPossessedCharacter()))
		{
			SevenCharacterToAttack = SevenCharacter;
			// Spawn emitter only if really attacking (enemy has a token)
			if (SevenCharacter->GetAttackTokenOwner() == uniqueID)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), IncomingAttackParticle,
					GetActorLocation(), FRotator(0, 0, 0), FVector(1, 1, 1), true, EPSCPoolMethod::None, true);
			}
		}
	}
}

void AEnemyCharacter::ParryAvailable(bool bEnable)
{
	if (SevenPlayerController)
	{
		SevenPlayerController->UpdateStatus(this, bEnable ? EEnemyStatus::ParryAvailable : EEnemyStatus::ParryUnavailable);
	}
}

void AEnemyCharacter::Fire(const FInputActionValue& Value)
{
	//Attack
	TargetedEnemy = GetClosestEnemyInRange();
	AC_AttackComponent->LightAttack(TargetedEnemy);

	if (TargetedEnemy)
	{
		UE_LOG(LogTemp, Error, TEXT("[AEnemyCharacter]Fire.TargetedEnemy %s"), *TargetedEnemy->GetName());
		//MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform("MW_LightAttackAttacker", TargetedEnemy->VictimDesiredPosition->GetComponentTransform());

		// Rotate character towards enemy
		FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetedEnemy->GetActorLocation());
		RootComponent->SetWorldRotation(PlayerRot);

		if (!AC_AttackComponent->LightAttack(TargetedEnemy))
		{
			AttackEnd();
		}
	}	
	ReturnAttackToken();
}

void AEnemyCharacter::ReceivedHit(const FAttackInfo& AttackInfo)
{
	UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter]ReceivedHit"));
	Super::ReceivedHit(AttackInfo);
	if (TargetedEnemy)
	{
		UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter]ReceivedHit.ReturningAttackToken"));
	}
	ReturnAttackToken();
}

void AEnemyCharacter::AttackEnd() const
{
	UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter] AttackEnd"));
	OnAttackEnd.Broadcast();
	SevenPlayerController->UpdateStatus(this, EEnemyStatus::Cooldown);
}

void AEnemyCharacter::OnLayingDead()
{
	SevenPlayerController->UpdateStatus(this, EEnemyStatus::Dead);
	Super::OnLayingDead();
}

void AEnemyCharacter::MoveTo(bool bToSevenCharacter, bool bBlockingStance)
{
	if (bBlockingStance)
	{
		AC_Animation->Block(true);
	}
	else
	{
		AC_Animation->Guard(true);
	}
	
	AAIController* EnemyController = Cast<AAIController>(GetController());
	ACharacter* SevenCharacter = SevenPlayerController->GetPossessedCharacter();

	if (!EnemyController)
	{
		return;
	}

	EnemyController->SetFocus(SevenCharacter);
	
	FVector FinalDestination;
	
	if (bToSevenCharacter)
	{
		if (SevenCharacter)
		{
			FinalDestination = SevenCharacter->GetActorLocation();
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
	
	EnemyController->MoveToLocation(FinalDestination, 150.0f);
}

const FVector AEnemyCharacter::GetRandomGuardPoint()
{
	const FVector Right = (FMath::RandBool() ? GetActorRightVector() : GetActorRightVector() * (-1)) * 400;
	const FVector Backwards = GetActorForwardVector() * (-1) * 400;

	const FVector FinalDestination = GetActorLocation() + Right + Backwards;
	return FinalDestination;
}

bool AEnemyCharacter::TryStealAttackToken()
{
	// TODO: This will be changed to a different logic (when there are more players) keep getting SevenCharacter from SevenPlayerController for now
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(SevenPlayerController->GetPossessedCharacter());
	if (!SevenCharacter)
	{
		return false;
	}
	if (SevenCharacter->CanStealAttackToken())
	{
		SevenCharacter->StealAttackToken(uniqueID);
		SevenCharacterToAttack = SevenCharacter;
		return true;
	}
	return false;
}

void AEnemyCharacter::ReturnAttackToken()
{
	if (SevenCharacterToAttack)
	{
		UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter].ReturnAttackToken Returning Attack Token"));
		SevenCharacterToAttack->ReturnAttackToken();
		SevenCharacterToAttack = nullptr;
	}
}





