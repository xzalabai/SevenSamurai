#pragma once

#include "CoreMinimal.h"
#include "VV_EntityBase.h"
#include "PublicEnums.h"
#include "Engine/DataTable.h"
#include "VV_CharacterUpgrade.generated.h"

// Combo with price-tag (specific for each shop)
USTRUCT(BlueprintType)
struct FComboWithPrice
{
	GENERATED_BODY(EditAnywhere)

	UPROPERTY(EditAnywhere)
	ESevenCharacterType SevenCharacterType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EComboType Type;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UObject> UObject;

	UPROPERTY(EditAnywhere)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* Image;
};

FORCEINLINE bool operator==(const FComboWithPrice& lhs, const FComboWithPrice& rhs)
{
	return lhs.Type == rhs.Type;
}

// DataAsset
UCLASS(BlueprintType)
class SEVEN_API UAllCombos : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FComboWithPrice> CombosList;
};

USTRUCT(BlueprintType)
struct SEVEN_API FVillageProperties : public FTableRowBase
{
	GENERATED_BODY()

	FVillageProperties() = default;
	FVillageProperties(int VillageID, TArray<FComboWithPrice> AvailableCombos, TArray<USevenCharacterDA*> AvailableCharacters)
		: VillageID(VillageID), AvailableCombos(AvailableCombos), AvailableCharacters(AvailableCharacters) {}

	UPROPERTY(EditAnywhere)
	int VillageID;

	UPROPERTY(EditAnywhere)
	TArray<FComboWithPrice> AvailableCombos;

	UPROPERTY(EditAnywhere)
	TArray<USevenCharacterDA*> AvailableCharacters;
};


UCLASS()
class SEVEN_API AVV_CharacterUpgrade : public AVV_EntityBase
{
	GENERATED_BODY()

	friend class AVV_Map;

protected:
	UPROPERTY(EditAnywhere)
	UAllCombos* AllCombos;

	UPROPERTY(BlueprintReadOnly)
	mutable TArray<FComboWithPrice> AvailableCombos;

public:
	virtual void OnOverlapAction() override;
	UFUNCTION(BlueprintImplementableEvent)
	void OnShopOverlapped(USevenCharacterDA* SevenCharacterDA);
	UFUNCTION(BlueprintCallable)
	void BuyCombo(const EComboType ComboType) const;
	const TArray<FComboWithPrice>& GenerateAvailableCombos(const TArray<USevenCharacterDA*> SevenCharactersDA);

protected:
	virtual void BeginPlay() override;
};
