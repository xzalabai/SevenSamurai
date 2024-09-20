// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VV_CharacterUpgrade.h"
#include "SevenCharacterData.generated.h"

class USevenCharacterDA;

UCLASS()
class SEVEN_API USevenCharacterData : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP;

	UPROPERTY(EditAnywhere)
	float MaxHP{ 3 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	mutable TArray<FComboWithPrice> Combos;

	UPROPERTY(EditDefaultsOnly)
	bool bWasUsed{ false };

	UPROPERTY(BlueprintReadOnly)
	mutable USevenCharacterDA* SevenCharacterDA;
	
};
