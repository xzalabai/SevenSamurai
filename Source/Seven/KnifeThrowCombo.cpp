#include "KnifeThrowCombo.h"
#include "ThrowingKnife.h"
#include "AnimationComponent.h"
#include "AttackComponent.h"
#include "SevenCharacter.h"

void UKnifeThrowCombo::Use(AActor* AttackerActor, AActor* VictimActor)
{
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(AttackerActor);
	if (SevenCharacter->AC_Animation->Play(ThrowAnimation, "Default", EMontageType::LightAttack, false))
	{
		Attacker = SevenCharacter;
	}
}

void UKnifeThrowCombo::ComboAttackStart()
{
	if (!Attacker)
	{
		UE_LOG(LogTemp, Error, TEXT("[UKnifeThrowCombo] ComboAttackStart"));
		return;
	}

	FVector Vector = Attacker->GetMesh()->GetSocketLocation("hand_rSocket");
	Vector = Vector + Attacker->GetActorForwardVector() * 50;
	const FRotator Rotation = FRotator::ZeroRotator;
	// TODO: try to put here CONST throwingKnife, create obj pool
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Owner = Attacker;
	TObjectPtr<AThrowingKnife> ThrowingKnife = GetWorld()->SpawnActor<AThrowingKnife>(ThrowingKnifeClass, Vector, Rotation, ActorSpawnParameters);
	EAttackType AttackType = Attacker->IsEnemy() ? EAttackType::Throw : EAttackType::Combo; // redefine it (due to incosistency between Throw of Enemy and Throw of Seven
	ThrowingKnife->AttackInfo = FAttackInfo(AttackType, Attacker->GetAttackStrength(), -1, 5, Attacker);
	ThrowingKnife->FireInDirection(Attacker->TargetedEnemy->GetActorLocation());
}

void UKnifeThrowCombo::ComboAttackEnd()
{

}

EComboType UKnifeThrowCombo::GetComboType()
{
    return ComboType;
}

void UKnifeThrowCombo::DealDamage(ASevenCharacter* Victim)
{
}
