#pragma once

#include "CoreMinimal.h"
#include "ComboInterface.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "Combo.generated.h"

class UAnimMontage; 
class ASevenCharacter;

UCLASS(Blueprintable, BlueprintType)
class SEVEN_API UCombo : public UObject, public IComboInterface
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere)
	EComboType ComboType;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AttackerAnimation;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* VictimAnimation;

public:
	virtual void Use(AActor* AttackerActor, AActor* VictimActor) override;
	virtual void ComboAttackStart() override;
	virtual void ComboAttackEnd() override;
	virtual EComboType GetComboType() const override;
	void DealDamage(ASevenCharacter* Victim) override;
};
