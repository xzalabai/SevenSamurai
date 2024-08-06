// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ComboInterface.h"
#include "UObject/NoExportTypes.h"
#include "KnifeThrowCombo.generated.h"

class ASevenCharacter;
class AThrowingKnife;

UCLASS(BlueprintType, Blueprintable)
class SEVEN_API UKnifeThrowCombo : public UObject, public IComboInterface
{
	GENERATED_BODY()
protected:

	TObjectPtr<ASevenCharacter> Attacker;

	UPROPERTY(EditAnywhere)
	EComboType ComboType;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* ThrowAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AThrowingKnife> ThrowingKnifeClass;

public:
	void Use(AActor* AttackerActor, AActor* VictimActor) override;
	void ComboAttackStart() override;
	void ComboAttackEnd() override;
	virtual EComboType GetComboType() override;
	void DealDamage(ASevenCharacter* Victim) override;
};
