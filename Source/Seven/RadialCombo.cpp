// Fill out your copyright notice in the Description page of Project Settings.

#include "RadialCombo.h"
#include "SevenCharacter.h"
#include "AnimationComponent.h"

void URadialCombo::Use(AActor* AttackerActor, AActor* VictimActor)
{
	ASevenCharacter* AttackerCharacter = Cast<ASevenCharacter>(AttackerActor);
	Attacker = AttackerCharacter;
	UE_LOG(LogTemp, Error, TEXT("[URadialCombo] Use"));
	UAnimationComponent* AC_Animation = AttackerCharacter->GetAnimationComponent();
	AC_Animation->Play(AttackerAnimation, "Default", EMontageType::Attack, false);
}

void URadialCombo::ComboAttackStart()
{
	if (!Attacker)
	{
		return;
	}
	UE_LOG(LogTemp, Error, TEXT("[URadialCombo] ComboAttackStart"));
	TArray<ASevenCharacter*> FoundEnemies = Attacker->GetEnemiesInFrontOfCharacer(-1, StartOffset, EndOffset, Thickness, true);
	FAttackInfo AttackInfo{ EAttackType::Combo, 0, BaseDamage, Attacker };
	for (ASevenCharacter* Enemy : FoundEnemies)
	{
		Enemy->ReceivedHit(AttackInfo);
	}
}

void URadialCombo::ComboAttackEnd()
{
	Attacker = nullptr;
}

const FName& URadialCombo::GetComboName()
{
	return ComboName;
}

