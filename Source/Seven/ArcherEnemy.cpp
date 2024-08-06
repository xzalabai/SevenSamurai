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
	UE_LOG(LogTemp, Display, TEXT("[AArcherEnemy]Fire"));

	AC_AttackComponent->SetCombo(1);
	TargetedEnemy = FindSevenCharacter();
	AC_AttackComponent->LightAttack(TargetedEnemy);
	ReturnAttackToken();
}

const FVector AArcherEnemy::GetRandomPointAroundCharacter(const ASevenCharacter* const SevenCharacter)
{
	const FVector EnemyPosition = SevenCharacter->GetActorLocation();
	const FVector EnemyForwardVector = SevenCharacter->GetActorForwardVector();

	const int Left = FMath::RandBool() ? (-1) : 1;
	//const int RandomOffsetX = FMath::RandRange(500, 1000);
	const int RandomOffsetX = FMath::RandRange(400, 800);
	const int RandomOffsetY = FMath::RandRange(400, 800) * Left;

	const FVector EnemyForwardVectorOffset = FVector(EnemyForwardVector.X + RandomOffsetX, EnemyForwardVector.Y + RandomOffsetY, EnemyForwardVector.Z);

	const FVector FinalDestination = FVector(EnemyPosition.X + EnemyForwardVectorOffset.X, EnemyPosition.Y + EnemyForwardVectorOffset.Y, EnemyPosition.Z);

	return FinalDestination;
}
