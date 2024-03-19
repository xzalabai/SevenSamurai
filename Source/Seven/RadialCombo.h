// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combo.h"
#include "RadialCombo.generated.h"

/**
 * 
 */
UCLASS()
class SEVEN_API URadialCombo : public UCombo
{
	GENERATED_BODY()
	virtual void Use(AActor* AttackerActor, AActor* VictimActor) override;
	virtual void ComboAttackStart() override;
	virtual void ComboAttackEnd() override;
};
