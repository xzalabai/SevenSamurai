// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RadialCombo.h"
#include "ShieldDodgeCombo.generated.h"

UCLASS()
class SEVEN_API UShieldDodgeCombo : public URadialCombo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EComboType Something;

	virtual void ComboAttackStart() override;
	virtual void ComboAttackEnd() override;
	
};
