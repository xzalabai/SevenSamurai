#include "VV_CharacterUpgrade.h"
#include "GameController.h"
#include "SevenCharacterDA.h"
#include "MV_PlayerController.h"
#include <Kismet\GameplayStatics.h>

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

	const TArray<USevenCharacterDA*> SevenCharactersDA = GameController->GetSelectedCharacters();
	USevenCharacterDA* SevenCharacterDA = SevenCharactersDA[0];

	MV_PlayerController->DisplayUIWindow(EUIWindow::ComboShop, true);

	//OnShopOverlapped(SevenCharacterDA);
	//BuyCombo(SevenCharacterDA, 0);
}

void AVV_CharacterUpgrade::BuyCombo(const EComboType ComboType) const
{
	const FComboWithPrice* ComboToBePurchased{ nullptr };
	int Index = 0;
	for (Index = 0; Index < AvailableCombos.Num(); ++Index)
	{
		if (AvailableCombos[Index].Type == ComboType)
		{
			ComboToBePurchased = &AvailableCombos[Index];
			break;
		}
	}

	if (!ComboToBePurchased)
	{
		UE_LOG(LogTemp, Error, TEXT("[AVV_CharacterUpgrade].BuyCombo Unable to find combo %d"), (int)ComboType);
		return;
	}

	TArray<USevenCharacterDA*> SelectedSevenCharacters = GameController->GetSelectedCharacters();
	const USevenCharacterDA* CharacterToUpgrade{ nullptr };
	for (const USevenCharacterDA* SevenCharacterDA : SelectedSevenCharacters)
	{
		if (SevenCharacterDA->SevenCharacterType == ComboToBePurchased->SevenCharacterType)
		{
			CharacterToUpgrade = SevenCharacterDA;
			break;
		}
	}
	if (!CharacterToUpgrade)
	{
		// TODO: Here should be some callback to UI (notify player that he has no suitable character for it)
		UE_LOG(LogTemp, Display, TEXT("[AVV_CharacterUpgrade].BuyCombo Unable to find suitable character to put combo to %d"), (int)ComboType);
		return;
	}
 
	if (CharacterToUpgrade->CombosObj.Num() > 2)
	{
		UE_LOG(LogTemp, Display, TEXT("[AVV_CharacterUpgrade].BuyCombo Unable to buy combo, Character %s already has 2 combos"), *CharacterToUpgrade->Name.ToString());
		return;
	}

	CharacterToUpgrade->CombosObj.Add((*ComboToBePurchased).UObject);
	CharacterToUpgrade->Combos.Add(*ComboToBePurchased);
	AvailableCombos.RemoveAt(Index);
	MV_PlayerController->UpdateUIWindow(EUIWindow::ComboShop);
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
	for (int i = 0; i < 150; ++i)
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
	}

	UE_LOG(LogTemp, Display, TEXT("[AVV_CharacterUpgrade].GenerateAvailableCombos Generated %d combos for the shop (with %d tries)"), ComboAmount, SafetyCheck);
	for (const FComboWithPrice& ComboWithPrice : AvailableCombos)
	{
		UE_LOG(LogTemp, Display, TEXT("[AVV_CharacterUpgrade].GenerateAvailableCombos GeneratedCombos: %s"), *UEnum::GetValueAsString(ComboWithPrice.Type));
	}

	return AvailableCombos;
}


