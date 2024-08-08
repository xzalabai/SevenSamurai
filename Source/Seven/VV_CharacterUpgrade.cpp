#include "VV_CharacterUpgrade.h"
#include "GameController.h"
#include "SevenCharacterDA.h"

void AVV_CharacterUpgrade::BeginPlay()
{
	Super::BeginPlay();
	const UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	const FName VillageID = CustomMath::IntToFName(GameController->GetVisitedVillageID());

	const FString ContextString;
	FAvailableCombosInVillages* RetrievedCombosInVillage = AvailableCombosInVillages->FindRow<FAvailableCombosInVillages>(VillageID, ContextString, true);

	if (RetrievedCombosInVillage)
	{
		UE_LOG(LogTemp, Error, TEXT("[AVV_CharacterUpgrade].BeginPlay Retrieving Combos!"));
		AvailableCombos = RetrievedCombosInVillage->CombosWithPrice;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[AVV_CharacterUpgrade].BeginPlay FIRST VILLAGE VISIT! Generating Combos"));
		GenerateAvailableCombos();
	}
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
	SevenCharacterDA->CombosObj.Add(ComboToBePurchased.Combo.UObject);
}

void AVV_CharacterUpgrade::GenerateAvailableCombos()
{
	int RandomNumber1 = 1;
	int RandomNumber2 = 3;

	int RandomPrice1 = 10;
	int RandomPrice2 = 30;

	FComboWithPrice ComboWithPrice1 = FComboWithPrice(AllCombos->CombosList[RandomNumber1], RandomPrice1);
	FComboWithPrice ComboWithPrice2 = FComboWithPrice(AllCombos->CombosList[RandomNumber2], RandomPrice2);

	AvailableCombos.Add(ComboWithPrice1);
	AvailableCombos.Add(ComboWithPrice2);

	// Write to DataTable
	FAvailableCombosInVillages AvailableCombosInVillage;
	AvailableCombosInVillage.CombosWithPrice = AvailableCombos;
	
	const UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>()); // nice cheat haha
	const FName VillageID = CustomMath::IntToFName(GameController->GetVisitedVillageID());
	AvailableCombosInVillages->AddRow(VillageID, AvailableCombosInVillage);
}


