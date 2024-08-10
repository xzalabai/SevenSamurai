// Fill out your copyright notice in the Description page of Project Settings.

#include "RadialCombo.h"
#include "SevenCharacter.h"
#include "AnimationComponent.h"

void URadialCombo::Use(AActor* AttackerActor, AActor* VictimActor)
{
	Attacker = Cast<ASevenCharacter>(AttackerActor);
	UAnimationComponent* AC_Animation = Attacker->GetAnimationComponent();
	AC_Animation->Play(AttackerAnimation, "Default", EMontageType::Combo);
}

void URadialCombo::ComboAttackStart()
{
	UE_LOG(LogTemp, Error, TEXT("[URadialCombo] ComboAttackStart"));
	TArray<ASevenCharacter*> FoundEnemies = Attacker->GetEnemiesInFrontOfCharacer(-1, StartOffset, EndOffset, Thickness, true);
	for (ASevenCharacter* Enemy : FoundEnemies)
	{
		Enemy->ReceivedHit(Attacker->GetAttackInfo());
	}
}

void URadialCombo::ComboAttackEnd()
{
	Attacker = nullptr;
}

EComboType URadialCombo::GetComboType() const
{
    return ComboType;
}