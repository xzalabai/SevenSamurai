#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PublicEnums.h"
#include "ComboInterface.generated.h"

class ASevenCharacter;

UINTERFACE(MinimalAPI)
class UComboInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SEVEN_API IComboInterface
{
	GENERATED_BODY()

public:
	virtual void Use(AActor* AttackerActor, AActor* VictimActor) = 0;
	virtual void ComboAttackStart() = 0;
	virtual void ComboAttackEnd() = 0;
	virtual EComboType GetComboType() const = 0;
	virtual void DealDamage(ASevenCharacter* Victim) = 0;
};
