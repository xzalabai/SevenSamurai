#include "VV_CharacterUpgrade.h"
#include "GameController.h"
#include "SevenCharacterDA.h"

void AVV_CharacterUpgrade::OnOverlapAction()
{
	// Display UI

	// For now, use lines :(

	UE_LOG(LogTemp, Display, TEXT("[AVV_CharacterUpgrade].OnOverlapAction Adding Combo"));

	int DebugIndex = 0; // TODO replace
	const UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	const TArray<USevenCharacterDA*> SevenCharactersDA = GameController->GetSelectedCharacters();
	USevenCharacterDA* SevenCharacterDA = SevenCharactersDA[0];
	const FSevenCharacterCombos SevenCharacterFCombos = SevenCharacterCombosMapping[SevenCharacterDA->SevenCharacterType];

	OnShopOverlapped(SevenCharacterDA);
}

void AVV_CharacterUpgrade::BuyCombo(USevenCharacterDA* SevenCharacterDA, int Index) const
{
	const FSevenCharacterCombos& SevenCharacterCombos = SevenCharacterCombosMapping[SevenCharacterDA->SevenCharacterType];

	if (SevenCharacterCombos.CombosAvailable.Num() < Index)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("[AVV_CharacterUpgrade].BuyCombo Combo with this INDEX is not amongst Available Combos!"));
		}
		UE_LOG(LogTemp, Error, TEXT("[AVV_CharacterUpgrade].BuyCombo Combo with this INDEX is not amongst Available Combos! %d"), Index);
		return;
	}
	const FCombosWithPrices& ComboToBePurchased = SevenCharacterCombos.CombosAvailable[Index];
	SevenCharacterDA->Combos.Add(ComboToBePurchased.Name); // TODO: maybe obsolete!
	SevenCharacterDA->CombosObj.Add(ComboToBePurchased.Combo);
}
