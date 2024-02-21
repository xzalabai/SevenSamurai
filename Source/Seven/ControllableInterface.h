#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputActionValue.h"
#include "ControllableInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UControllableInterface : public UInterface
{
	GENERATED_BODY()
};

class SEVEN_API IControllableInterface
{
	GENERATED_BODY()
public:
	virtual void Move(const FInputActionValue& Value) = 0;
	virtual void Look(const FInputActionValue& Value) = 0;
	virtual void Space(const FInputActionValue& Value) = 0;
	virtual void StopSpace(const FInputActionValue& Value) = 0;
	virtual void Fire(const FInputActionValue& Value) = 0;
	virtual void Block(bool bEnable) = 0;
	virtual void Evade(const FInputActionValue& Value) = 0;
	virtual void Special(int ID) = 0;
	virtual void AttackStart() = 0;
	virtual void AttackEnd() = 0;

public:
};
