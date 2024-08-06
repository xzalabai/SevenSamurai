// Fill out your copyright notice in the Description page of Project Settings.

#include "RadialCombo.h"
#include "SevenCharacter.h"
#include "AnimationComponent.h"

void URadialCombo::Use(AActor* AttackerActor, AActor* VictimActor)
{
	Attacker = Cast<ASevenCharacter>(AttackerActor);
	UAnimationComponent* AC_Animation = Attacker->GetAnimationComponent();
	AC_Animation->Play(AttackerAnimation, "Default", EMontageType::LightAttack, false);
}

void URadialCombo::ComboAttackStart()
{
	UE_LOG(LogTemp, Error, TEXT("[URadialCombo] ComboAttackStart"));
	TArray<ASevenCharacter*> FoundEnemies = Attacker->GetEnemiesInFrontOfCharacer(-1, StartOffset, EndOffset, Thickness, true);
	FAttackInfo AttackInfo{ EAttackType::Combo, EAttackStrength::Light, 0, BaseDamage, Attacker };
	for (ASevenCharacter* Enemy : FoundEnemies)
	{
		Enemy->ReceivedHit(AttackInfo);
	}
}

void URadialCombo::ComboAttackEnd()
{
	Attacker = nullptr;
}

EComboType URadialCombo::GetComboType()
{
    return ComboType;
}

void URadialCombo::DealDamage(ASevenCharacter* Victim)
{
}

