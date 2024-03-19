#pragma once

#include "CoreMinimal.h"
#include "ComboInterface.h"
#include "UObject/NoExportTypes.h"
#include "Combo.generated.h"

class UAnimMontage; 
class ASevenCharacter;

UCLASS(Blueprintable, BlueprintType)

class SEVEN_API UCombo : public UObject
{
	GENERATED_BODY()

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
//////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	UPROPERTY(EditDefaultsOnly)
	FName ComboName;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AttackerAnimation;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* VictimAnimation;

	TObjectPtr<ASevenCharacter> Attacker;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	virtual void Use(AActor* AttackerActor, AActor* VictimActor);
	virtual void ComboAttackStart();
	virtual void ComboAttackEnd();
};
