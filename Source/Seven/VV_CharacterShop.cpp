#include "VV_CharacterShop.h"
#include "GameController.h"
#include "SevenCharacterDA.h"


void AVV_CharacterShop::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("[AVV_CharacterShop].BeginPlay"));
}

const TArray<USevenCharacterDA*>& AVV_CharacterShop::GenerateAvailableCharacters()
{
	int AmountOfAvailableCharacters = FMath::RandRange(1, 3);
	int SafetyCheck{ 100 };
	while (1)
	{
		const int ComboIndex = FMath::RandRange(0, AllCharacters.Num() - 1);
		USevenCharacterDA* SevenCharacterDA = AllCharacters[ComboIndex];

		if (SevenCharacterDA->bWasUsed && AvailableCharacters.Find(SevenCharacterDA) != INDEX_NONE)
		{
			continue;
		}

		if (AmountOfAvailableCharacters == AvailableCharacters.Num())
		{
			break;
		}

		AvailableCharacters.Add(SevenCharacterDA);

		++SafetyCheck;
		if (SafetyCheck > 150)
		{
			// No more saved character? Back luck amigo
			UE_LOG(LogTemp, Fatal, TEXT("[AVV_CharacterUpgrade].GenerateAvailableCombos Too many iterations during generation %d"), SafetyCheck);
			return AvailableCharacters;
		}
	}
	return AvailableCharacters;
}


void AVV_CharacterShop::OnOverlapAction()
{
	// UI
	PickCharacter(0);
}

void AVV_CharacterShop::PickCharacter(const int Index)
{
	const UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	USevenCharacterDA* PickedCharacter = AvailableCharacters[Index];
	PickedCharacter->bWasUsed = true;
	GameController->AddToSelectedCharacter(PickedCharacter);
}
