#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameController.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnRestart);

UCLASS()
class SEVEN_API UGameController : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	FOnRestart OnRestart;

public:

	UFUNCTION()
	void Restart();
};
