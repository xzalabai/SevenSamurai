#pragma once

#include "CoreMinimal.h"
#include "VV_EntityBase.h"
#include "PublicEnums.h"
#include "VV_CharacterUpgrade.generated.h"

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
class SEVEN_API AVV_CharacterUpgrade : public AVV_EntityBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
	TMap<ESevenCharacterType, FSevenCharacterCombos> SevenCharacterCombosMapping;

public:
	virtual void OnOverlapAction() override;
	UFUNCTION(BlueprintImplementableEvent)
	void OnShopOverlapped(USevenCharacterDA* SevenCharacterDA);
	UFUNCTION(BlueprintCallable)
	void BuyCombo(USevenCharacterDA* SevenCharacterDA, int Index) const;
};
