#pragma once

#include "CoreMinimal.h"
#include "VV_EntityBase.h"
#include "PublicEnums.h"
#include "Engine/DataTable.h"
#include "VV_CharacterUpgrade.generated.h"

UENUM(BlueprintType)
enum class EComboType : uint8
{
	ECT_None,
	ECT_Radial,
	ECT_AttackSequence,
	ECT_Heavy,
	ECT_Throw,
};

// Definition of Combo (equal for every shop)
USTRUCT(BlueprintType)
struct FComboDefiniton
{
	GENERATED_BODY(EditAnywhere)

	UPROPERTY(EditAnywhere)
	ESevenCharacterType SevenCharacterType;

	UPROPERTY(EditAnywhere)
	EComboType Type;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UObject> UObject;

	UPROPERTY(EditAnywhere)
	FName Name;
};

// Combo with price-tag (specific for each shop)
USTRUCT(BlueprintType)
struct FComboWithPrice
{
	GENERATED_BODY()
	FComboWithPrice() = default;
	FComboWithPrice(FComboDefiniton _Combo, int _Price) : Combo(_Combo), Price(_Price) {}

	UPROPERTY(EditAnywhere)
	FComboDefiniton Combo;

	UPROPERTY(EditAnywhere)
	int Price;
};

// DataAsset
UCLASS(BlueprintType)
class SEVEN_API UAllCombos : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FComboDefiniton> CombosList;
};

USTRUCT(BlueprintType)
struct SEVEN_API FAvailableCombosInVillages : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FComboWithPrice> CombosWithPrice;
};

UCLASS()
class SEVEN_API AVV_CharacterUpgrade : public AVV_EntityBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	UAllCombos* AllCombos;

	UPROPERTY()
	TArray<FComboWithPrice> AvailableCombos;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> AvailableCombosInVillages;


public:
	virtual void OnOverlapAction() override;
	UFUNCTION(BlueprintImplementableEvent)
	void OnShopOverlapped(USevenCharacterDA* SevenCharacterDA);
	UFUNCTION(BlueprintCallable)
	void BuyCombo(USevenCharacterDA* SevenCharacterDA, int Index) const;
	void GenerateAvailableCombos();

protected:
	virtual void BeginPlay() override;

};
