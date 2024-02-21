// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyController.generated.h"

/**
 * 
 */
UCLASS()
class SEVEN_API AEnemyController : public AAIController
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTree* AIBehavior;
};
