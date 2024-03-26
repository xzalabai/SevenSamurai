// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SevenGameMode.generated.h"

class ULootGenerator;

UCLASS(minimalapi)
class ASevenGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASevenGameMode();
	
	UPROPERTY()
	ULootGenerator* AC_LootGenerator;
};



