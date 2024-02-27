#include "EnemyCharacter.h"
#include "Weapon.h"
#include "AnimationComponent.h"
#include "SevenPlayerController.h"
#include "ComboManager.h"
#include "Kismet\GameplayStatics.h"

void AEnemyCharacter::InitiateAttack()
{
	Fire(FInputActionValue());
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	SevenPlayerController = Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

void AEnemyCharacter::IncomingAttack()
{
	if (SevenPlayerController)
	{
		SevenPlayerController->UpdateStatus(GetUniqueID(), EEnemyStatus::IncomingAttack);
	}
}

void AEnemyCharacter::ParryAvailable(bool bEnable)
{
	if (SevenPlayerController)
	{
		SevenPlayerController->UpdateStatus(GetUniqueID(), bEnable ? EEnemyStatus::ParryAvailable : EEnemyStatus::ParryUnavailable);
	}
}

void AEnemyCharacter::Fire(const FInputActionValue& Value)
{
	// Attack
	EquippedWeapon->ClearHitActors();

	if (ComboComponent->SpecialActivated == ESpecial::ES_Special1)
	{
		ComboComponent->UseCombo(ESpecial::ES_Special1);
	}
	else
	{
		TargetedEnemy = GetClosestEnemyInRange();
		if (TargetedEnemy)
		{
			UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter]Fire.TargetedEnemy %s"), *TargetedEnemy->GetName());
			//MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform("MW_LightAttackAttacker", TargetedEnemy->VictimDesiredPosition->GetComponentTransform());
		}

		int RandomMontage = FMath::RandRange(1, LightAttackAttacker->CompositeSections.Num());
		FString RandomMontageStr = FString::FromInt(RandomMontage);
		AC_Animation->Play(LightAttackAttacker, FName(*RandomMontageStr), false);
	}

	// Subscribe to Player Controller
	SevenPlayerController->UpdateStatus(GetUniqueID());
}

void AEnemyCharacter::AttackEnd() const
{
	OnAttackEnd.Broadcast();
	SevenPlayerController->UpdateStatus(GetUniqueID(), EEnemyStatus::Cooldown);
}

