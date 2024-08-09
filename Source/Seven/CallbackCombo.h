// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combo.h"
#include "ComboInterface.h"
#include "CallbackCombo.generated.h"

class ASevenCharacter;

UCLASS(Blueprintable, BlueprintType)
class SEVEN_API UCallbackCombo : public UObject, public IComboInterface
{
	GENERATED_BODY()
protected:
	TObjectPtr<ASevenCharacter> Attacker;

	UPROPERTY(EditAnywhere)
	EComboType ComboType;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AttackerAnimation;
public:
	virtual void Use(AActor* AttackerActor, AActor* VictimActor) override;
	virtual void ComboAttackStart() override;
	virtual void ComboAttackEnd() override;
	virtual EComboType GetComboType() const override;
	void DealDamage(ASevenCharacter* Victim) override;
	
};
