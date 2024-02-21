#include "EnemyCharacter.h"
#include "Weapon.h"
#include "AnimationComponent.h"
#include "ComboManager.h"

void AEnemyCharacter::InitiateAttack()
{
	Fire(FInputActionValue());
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
}

void AEnemyCharacter::AttackEnd()
{
	OnAttackEnd.Broadcast();
}
