// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PublicEnums.h"
#include "Weapon.h"
#include "Engine/DataAsset.h"
#include "SevenCharacterDA.generated.h"

class ASevenCharacter;

UCLASS(BlueprintType)
class SEVEN_API USevenCharacterDA : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HP;

	UPROPERTY(EditAnywhere)
	float MaxHP{ 3 };

	UPROPERTY(EditAnywhere)
	FName Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Image;

	UPROPERTY(EditAnywhere)
	ESevenCharacterType SevenCharacterType;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASevenCharacter> RepresentingClass;

	UPROPERTY(EditAnywhere)
	FWeaponDetail WeaponDetail;

	UPROPERTY(EditAnywhere, meta = (MustImplement = "ComboInterface"))
	TArray<TSubclassOf<UObject>> CombosObj;

	UPROPERTY(EditDefaultsOnly)
	bool bWasUsed{ false };
};
