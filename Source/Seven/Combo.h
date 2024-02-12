// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Combo.generated.h"

class UAnimMontage;
class ASevenCharacter;

UCLASS()
class SEVEN_API ACombo : public AActor
{
	GENERATED_BODY()
	
public:	
	ACombo();
	virtual void Use(AActor* Attacker, AActor* Victim);

// Variables

protected:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AttackerAnimation;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* VictimAnimation;

	UPROPERTY(EditDefaultsOnly)
	FName Name;

};
