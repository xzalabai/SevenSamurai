#include "VV_CharacterShop.h"
#include "GameController.h"
#include "SevenPlayerController.h"
#include "SevenCharacterDA.h"
#include <Kismet\GameplayStatics.h>
#include "AttributesComponent.h"
#include "MV_PlayerController.h"


void AVV_CharacterShop::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("[AVV_CharacterShop].BeginPlay"));
}

const TArray<USevenCharacterDA*>& AVV_CharacterShop::GenerateAvailableCharacters()
{
	const int AmountOfAvailableCharacters = FMath::RandRange(1, 2);
	int SafetyCheck{ 0 };
	for (int i = 0; i < 150; ++i)
	{
		++SafetyCheck;
		if (SafetyCheck > 150)
		{
			// No more saved character? Back luck amigo
			UE_LOG(LogTemp, Error, TEXT("[AVV_CharacterUpgrade].GenerateAvailableCombos Too many iterations during generation %d"), SafetyCheck);
			break;
		}

		const int ComboIndex = FMath::RandRange(0, AllCharacters.Num() - 1);
		USevenCharacterDA* SevenCharacterDA = AllCharacters[ComboIndex];

		if (SevenCharacterDA->bWasUsed || AvailableCharacters.Find(SevenCharacterDA) != INDEX_NONE)
		{
			continue;
		}

		if (AmountOfAvailableCharacters == AvailableCharacters.Num())
		{
			break;
		}

		AvailableCharacters.Add(SevenCharacterDA);
	}

	UE_LOG(LogTemp, Display, TEXT("[AVV_CharacterShop].GenerateAvailableCharacters!"));
	for (USevenCharacterDA* SevenCharacterDA : AvailableCharacters)
	{
		UE_LOG(LogTemp, Display, TEXT("[AVV_CharacterShop].GenerateAvailableCharacters for the Shop: %s"), *SevenCharacterDA->Name.ToString());
	}
	return AvailableCharacters;
}

void AVV_CharacterShop::RemoveFromPlayersCharacters(USevenCharacterDA* SevenCharacterDA)
{
	GameController->SelectedCharacters.Remove(SevenCharacterDA);
	if (GameController->SelectedCharacters.Num() == 1)
	{
		// Minimum 1 character
		return;
	}
	AvailableCharacters.Add(SevenCharacterDA);
	
	MV_PlayerController->UpdateUIWindow(EUIWindow::CharacterShop);
}

void AVV_CharacterShop::RemoveFromShopCharacters(USevenCharacterDA* SevenCharacterDA)
{
	if (GameController->SelectedCharacters.Num() == 3)
	{
		// Maximum 3 sevenCharacters!
		return;
	}

	GameController->SelectedCharacters.Add(SevenCharacterDA);
	AvailableCharacters.Remove(SevenCharacterDA);

	MV_PlayerController->UpdateUIWindow(EUIWindow::CharacterShop);
}


void AVV_CharacterShop::OnOverlapAction()
{
	MV_PlayerController->DisplayUIWindow(EUIWindow::CharacterShop, true);
	// UI
	//PickCharacter(0);
}

void AVV_CharacterShop::PickCharacter(const int Index)
{
	USevenCharacterDA* PickedCharacter = AvailableCharacters[Index];
	PickedCharacter->bWasUsed = true;
	GameController->AddToSelectedCharacter(PickedCharacter);
}
