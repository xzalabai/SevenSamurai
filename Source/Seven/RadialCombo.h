// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combo.h"
#include "ComboInterface.h"
#include "RadialCombo.generated.h"

/**
 * 
 */
UCLASS()
class SEVEN_API URadialCombo : public UObject, public IComboInterface
{

	GENERATED_BODY()
	
	TObjectPtr<ASevenCharacter> Attacker;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AttackerAnimation;

	UPROPERTY(EditDefaultsOnly)
	FName ComboName;

	UPROPERTY(EditDefaultsOnly)
	uint8 BaseDamage{ 0 };

	UPROPERTY(EditDefaultsOnly)
	int32 StartOffset{ 0 };

	UPROPERTY(EditDefaultsOnly)
	int32 EndOffset{ 0 };

	UPROPERTY(EditDefaultsOnly)
	int32 Thickness{ 0 };

	
	virtual void Use(AActor* AttackerActor, AActor* VictimActor) override;
	virtual void ComboAttackStart() override;
	virtual void ComboAttackEnd() override;
	virtual const FName& GetComboName() override;
};
