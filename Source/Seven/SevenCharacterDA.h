// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PublicEnums.h"
#include "Weapon.h"
#include "Engine/DataAsset.h"
#include "SevenCharacterDA.generated.h"

class ASevenCharacter;

UCLASS()
class SEVEN_API USevenCharacterDA : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	FName Name;

	UPROPERTY(EditAnywhere)
	FName Description;

	UPROPERTY(EditAnywhere)
	UTexture2D* Image;

	UPROPERTY(EditAnywhere)
	ESevenCharacterType SevenCharacterType;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASevenCharacter> RepresentingClass;

	UPROPERTY(EditAnywhere)
	FWeaponDetail WeaponDetail;

	UPROPERTY(EditAnywhere)
	TArray<FName> Combos;
};
