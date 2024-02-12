#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ComboInterface.generated.h"

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
	virtual void Use() = 0;
public:
};
