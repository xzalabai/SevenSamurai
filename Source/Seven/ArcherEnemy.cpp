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
	ASevenCharacter* EnemyToAttack = GetClosestEnemyInRange();
	AC_AttackComponent->LightAttack(EnemyToAttack);
	UE_LOG(LogTemp, Display, TEXT("[AArcherEnemy]Fire"));
	ReturnAttackToken();
}
