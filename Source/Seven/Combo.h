#pragma once

#include "CoreMinimal.h"
#include "ComboInterface.h"
#include "UObject/NoExportTypes.h"
#include "Combo.generated.h"

class UAnimMontage; 
class ASevenCharacter;

UCLASS(Blueprintable, BlueprintType)

class SEVEN_API UCombo : public UObject, public IComboInterface
{
	GENERATED_BODY()

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
//////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	UPROPERTY(EditDefaultsOnly)
	FName ComboName;

protected:

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AttackerAnimation;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* VictimAnimation;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	virtual void Use(AActor* AttackerActor, AActor* VictimActor) override;
	virtual void ComboAttackStart() override;
	virtual void ComboAttackEnd() override;
	virtual const FName& GetComboName() override;
};
