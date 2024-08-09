#include "KnifeThrowCombo.h"
#include "ThrowingKnife.h"
#include "AnimationComponent.h"
#include "AttackComponent.h"
#include "SevenCharacter.h"

void UKnifeThrowCombo::Use(AActor* AttackerActor, AActor* VictimActor)
{
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(AttackerActor);
	if (SevenCharacter->AC_Animation->Play(ThrowAnimation, "Default", EMontageType::Combo))
	{
		Attacker = SevenCharacter;
	}
}

void UKnifeThrowCombo::ComboAttackStart()
{
	ASevenCharacter* TargetedEnemy = Attacker->TargetedEnemy;

	if (!TargetedEnemy)
	{
		const TArray<ASevenCharacter*> FoundEnemies = Attacker->GetEnemiesInFrontOfCharacer(-1, StartOffset, EndOffset, Thickness, true);
		if (FoundEnemies.Num() == 0)
		{
			Attacker->ComboAttackEnd();
			return;
		}
		TargetedEnemy = FoundEnemies[0];
	}

	const FVector Vector = Attacker->GetMesh()->GetSocketLocation("hand_rSocket") + (Attacker->GetActorForwardVector() * 50);
	const FRotator Rotation = FRotator::ZeroRotator;
	// TODO: try to put here CONST throwingKnife, create obj pool
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Owner = Attacker;
	TObjectPtr<AThrowingKnife> const ThrowingKnife = GetWorld()->SpawnActor<AThrowingKnife>(ThrowingKnifeClass, Vector, Rotation, ActorSpawnParameters);
	EMontageType MontageType = Attacker->IsEnemy() ? EMontageType::Throw : EMontageType::Combo; // redefine it (due to incosistency between Throw of Enemy and Throw of Seven
	ThrowingKnife->AttackInfo = FAttackInfo(MontageType, Attacker->GetAttackStrength(), -1, 5, Attacker, EComboType::Throw);
	ThrowingKnife->FireInDirection(TargetedEnemy->GetActorLocation());
}

void UKnifeThrowCombo::ComboAttackEnd()
{

}

EComboType UKnifeThrowCombo::GetComboType() const
{
    return ComboType;
}

void UKnifeThrowCombo::DealDamage(ASevenCharacter* Victim)
{
}
