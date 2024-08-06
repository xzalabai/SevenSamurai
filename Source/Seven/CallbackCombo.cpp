#include "CallbackCombo.h"
#include "Shield.h"
#include "AnimationComponent.h"
#include "SevenCharacter.h"

void UCallbackCombo::Use(AActor* AttackerActor, AActor* VictimActor)
{
	Attacker = Cast<ASevenCharacter>(AttackerActor);
	UAnimationComponent* AC_Animation = Attacker->GetAnimationComponent();
	AC_Animation->Play(AttackerAnimation, "Default", EMontageType::LightAttack, false);
}

void UCallbackCombo::ComboAttackStart()
{
	UE_LOG(LogTemp, Error, TEXT("[AShield] ComboAttackStart"));
	Attacker->EquippedShield->EnableShieldHits(true);
}

void UCallbackCombo::ComboAttackEnd()
{
	UE_LOG(LogTemp, Error, TEXT("[AShield] ComboAttackEnd"));
	Attacker->EquippedShield->EnableShieldHits(false);
}

EComboType UCallbackCombo::GetComboType()
{
	return ComboType;
}

void UCallbackCombo::DealDamage(ASevenCharacter* Victim)
{
	Victim->ReceivedHit(FAttackInfo(EAttackType::Heavy, EAttackStrength::Heavy, 0, 10, Attacker));
	Attacker->EquippedShield->EnableShieldHits(false);
}