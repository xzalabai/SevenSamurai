// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcherEnemy.h"
#include "AttackComponent.h"
#include "Kismet\KismetMathLibrary.h"

void AArcherEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AArcherEnemy::Fire(const FInputActionValue& Value)
{
	//Attack
	AC_AttackComponent->SetCombo(1);
	TargetedEnemy = GetClosestEnemyInRange();
	AC_AttackComponent->LightAttack(TargetedEnemy);
	UE_LOG(LogTemp, Display, TEXT("[AArcherEnemy]Fire"));
	if (TargetedEnemy)
	{
		UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter]Fire.TargetedEnemy %s"), *TargetedEnemy->GetName());
		TargetedEnemy->ReturnAttackToken();
	}
}

void AArcherEnemy::TestM()
{
	UE_LOG(LogTemp, Error, TEXT("[AArcherEnemy]AArcherEnemy"));
}
