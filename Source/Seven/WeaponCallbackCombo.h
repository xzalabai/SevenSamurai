#pragma once

#include "CoreMinimal.h"
#include "CallbackCombo.h"
#include "WeaponCallbackCombo.generated.h"

UCLASS()
class SEVEN_API UWeaponCallbackCombo : public UCallbackCombo
{
	GENERATED_BODY()
	
public:
	virtual void Use(AActor* AttackerActor, AActor* VictimActor) override;
	virtual void ComboAttackStart() override;
	virtual void ComboAttackEnd() override;
	virtual EComboType GetComboType() const override;
};
