// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PublicEnums.h"
#include "EnemyLevelDA.generated.h"

UENUM(BlueprintType)
enum class EEnemyLevel : uint8
{
	Level1,
	Level2,
	Level3,
	Level4,
	Level5,
};

UCLASS(Blueprintable, BlueprintType)
class SEVEN_API UEnemyLevelDA : public UDataAsset
{
	GENERATED_BODY()
public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EEnemyLevel EnemyLevel{ EEnemyLevel::Level1 };
	 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MaxLightAttackAmount{ 1 };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefendChance{ 0.5 };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefendByBlockChance{ 0.5 }; // or evade

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EAttackStrength AllowedAttackStrengths;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ComboUsage{ 0.5 };
};