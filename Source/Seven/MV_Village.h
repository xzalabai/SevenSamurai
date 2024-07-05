// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MV_EntityBase.h"
#include "MV_Village.generated.h"

/**
 * 
 */
UCLASS()
class SEVEN_API AMV_Village : public AMV_EntityBase
{
	GENERATED_BODY()

public:
	virtual void OnOverlapAction() override;
};
