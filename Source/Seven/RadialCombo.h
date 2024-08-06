// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combo.h"
#include "ComboInterface.h"
#include "RadialCombo.generated.h"

class ASevenCharacter;

UCLASS()
class SEVEN_API URadialCombo : public UObject, public IComboInterface
{

	GENERATED_BODY()
protected:
	TObjectPtr<ASevenCharacter> Attacker;

	UPROPERTY(EditAnywhere)
	EComboType ComboType;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AttackerAnimation;

private:
	UPROPERTY(EditDefaultsOnly)
	uint8 BaseDamage{ 0 };

	UPROPERTY(EditDefaultsOnly)
	int32 StartOffset{ 0 };

	UPROPERTY(EditDefaultsOnly)
	int32 EndOffset{ 0 };

	UPROPERTY(EditDefaultsOnly)
	int32 Thickness{ 0 };

public:
	virtual void Use(AActor* AttackerActor, AActor* VictimActor) override;
	virtual void ComboAttackStart() override;
	virtual void ComboAttackEnd() override;
	virtual EComboType GetComboType() override;
	void DealDamage(ASevenCharacter* Victim) override;
};
