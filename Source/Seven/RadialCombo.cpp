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
	TArray<ASevenCharacter*> FoundEnemies = Attacker->GetEnemiesInFrontOfCharacer(-1, 0, 600, 300, true);
	FAttackInfo AttackInfo{ EAttackType::Combo, 0, 10, Attacker };
	for (ASevenCharacter* Enemy : FoundEnemies)
	{
		Enemy->ReceivedHit(AttackInfo);
	}
}

void URadialCombo::ComboAttackEnd()
{
	Attacker = nullptr;
}

