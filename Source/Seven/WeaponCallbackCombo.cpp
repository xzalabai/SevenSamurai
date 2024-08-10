#include "WeaponCallbackCombo.h"
#include "SevenCharacter.h"
#include "Weapon.h"
#include "AnimationComponent.h"


void UWeaponCallbackCombo::Use(AActor* AttackerActor, AActor* VictimActor)
{
	Attacker = Cast<ASevenCharacter>(AttackerActor);
	UAnimationComponent* AC_Animation = Attacker->GetAnimationComponent();
	AC_Animation->Play(AttackerAnimation, "Default", EMontageType::Combo);
}

void UWeaponCallbackCombo::ComboAttackStart()
{
	Attacker->EquippedWeapon->AttackStart();
}

void UWeaponCallbackCombo::ComboAttackEnd()
{

}

EComboType UWeaponCallbackCombo::GetComboType() const
{
	return ComboType;
}