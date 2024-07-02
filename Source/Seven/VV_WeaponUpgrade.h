// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "VV_EntityBase.h"
#include "VV_WeaponUpgrade.generated.h"

/**
 * 
 */
UCLASS()
class SEVEN_API AVV_WeaponUpgrade : public AVV_EntityBase
{
	GENERATED_BODY()

public:
	virtual void OnOverlapAction() override;
};
