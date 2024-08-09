// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacter.h"
#include "ArcherEnemy.generated.h"

/**
 * 
 */
UCLASS()
class SEVEN_API AArcherEnemy : public AEnemyCharacter
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay();
	virtual const FVector GetRandomPointAroundCharacter(const ASevenCharacter* const SevenCharacter) override;
};
