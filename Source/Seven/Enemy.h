// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SevenCharacter.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class SEVEN_API AEnemy : public ASevenCharacter
{
	GENERATED_BODY()
	AEnemy();

private:
	UPROPERTY()
	ASevenCharacter* MainCharacter;
public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	

	
};
