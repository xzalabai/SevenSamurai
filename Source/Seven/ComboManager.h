#pragma once

#include "CoreMinimal.h"
#include "PublicEnums.h"
#include "GameFramework/Actor.h"
#include "ComboManager.generated.h"

class ASevenCharacter;
class UCombo;

USTRUCT(BlueprintType)
struct FCombosWithPrices
{
	GENERATED_BODY()

	FCombosWithPrices() = default;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UObject> Combo;

	UPROPERTY(EditAnywhere)
	int Price;

	UPROPERTY(EditAnywhere)
	FName Name;
};

USTRUCT(BlueprintType)
struct FSevenCharacterCombos
{
	GENERATED_BODY()

	FSevenCharacterCombos() = default;
	UPROPERTY(EditAnywhere)
	TArray<FCombosWithPrices> CombosAvailable;
};

UCLASS()
class SEVEN_API AComboManager : public AActor
{
	GENERATED_BODY()
};


