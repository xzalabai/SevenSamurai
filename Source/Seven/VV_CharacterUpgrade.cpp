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


	const FSevenCharacterCombos SevenCharacterCombos = SevenCharacterCombosMapping[SevenCharacterDA->SevenCharacterType];
	const FCombosWithPrices ComboToBePurchased = SevenCharacterCombos.CombosAvailable[DebugIndex];

	SevenCharacterDA->Combos.Add(ComboToBePurchased.Name);
	SevenCharacterDA->CombosObj.Add(ComboToBePurchased.Combo);
}
