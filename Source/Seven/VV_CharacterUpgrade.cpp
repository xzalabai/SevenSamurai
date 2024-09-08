#include "VV_CharacterUpgrade.h"
#include "GameController.h"
#include "SevenCharacterDA.h"

void AVV_CharacterUpgrade::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("[AVV_CharacterUpgrade].BeginPlay"));
}

void AVV_CharacterUpgrade::OnOverlapAction()
{
	// Display UI

	// For now, use lines :(

	UE_LOG(LogTemp, Display, TEXT("[AVV_CharacterUpgrade].OnOverlapAction Adding Combo"));

	const UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	const TArray<USevenCharacterDA*> SevenCharactersDA = GameController->GetSelectedCharacters();
	USevenCharacterDA* SevenCharacterDA = SevenCharactersDA[0];

	OnShopOverlapped(SevenCharacterDA);
}

void AVV_CharacterUpgrade::BuyCombo(USevenCharacterDA* SevenCharacterDA, int Index) const
{
	const FComboWithPrice& ComboToBePurchased = AvailableCombos[Index];

	if (SevenCharacterDA->CombosObj.Num() > 2)
	{
		UE_LOG(LogTemp, Display, TEXT("[AVV_CharacterUpgrade].BuyCombo Unable to buy combo, Character %s already has 2 combos"), *SevenCharacterDA->Name.ToString());
		return;
	}

	SevenCharacterDA->CombosObj.Add(ComboToBePurchased.UObject);
}

const TArray<FComboWithPrice>& AVV_CharacterUpgrade::GenerateAvailableCombos(const TArray<USevenCharacterDA*> SevenCharactersDA)
{
	TArray<ESevenCharacterType> SevenCharactersTypes;
	SevenCharactersTypes.Reserve(3);

	for (const USevenCharacterDA* SevenCharacterDA : SevenCharactersDA)
	{
		SevenCharactersTypes.Add(SevenCharacterDA->SevenCharacterType);
	}

	const int ComboAmount = FMath::RandRange(1, 4);
	bool bSuitableCombo = false;
	int SafetyCheck = 0;
	while (1)
	{
		const int ComboIndex = FMath::RandRange(0, AllCombos->CombosList.Num() - 1);

		if (!bSuitableCombo)
		{
			// First satisfy the condition that shop must have combo suitable for any of the characters
			if (SevenCharactersTypes.Find(AllCombos->CombosList[ComboIndex].SevenCharacterType) != INDEX_NONE)
			{
				AvailableCombos.Add(AllCombos->CombosList[ComboIndex]);
				bSuitableCombo = true;
			}
			continue;
		}

		if (ComboAmount == AvailableCombos.Num())
		{
			break;
		}

		if (AvailableCombos.Find(AllCombos->CombosList[ComboIndex]) != INDEX_NONE)
		{
			continue;
		}

		AvailableCombos.Add(AllCombos->CombosList[ComboIndex]);

		++SafetyCheck;
		if (SafetyCheck > 150)
		{
			UE_LOG(LogTemp, Fatal, TEXT("[AVV_CharacterUpgrade].GenerateAvailableCombos Too many iterations during generation %d"), SafetyCheck);
			break;
		}
	}

	UE_LOG(LogTemp, Display, TEXT("[AVV_CharacterUpgrade].GenerateAvailableCombos Generated %d combos for the shop (with %d tries)"), ComboAmount, SafetyCheck);
	for (const FComboWithPrice& ComboWithPrice : AvailableCombos)
	{
		UE_LOG(LogTemp, Display, TEXT("[AVV_CharacterUpgrade].GenerateAvailableCombos GeneratedCombos: %s"), *UEnum::GetValueAsString(ComboWithPrice.Type));
	}

	return AvailableCombos;
}


