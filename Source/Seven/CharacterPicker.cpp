#include "CharacterPicker.h"
#include "SevenCharacter.h"
#include "SevenCharacterDA.h"
#include "GameController.h"
#include "Weapon.h"
#include "Mission.h"
#include "Kismet\GameplayStatics.h"

ACharacterPicker::ACharacterPicker()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ACharacterPicker::BeginPlay()
{
	Super::BeginPlay();
	SpawnSevenCharacters();
}

void ACharacterPicker::ShowAvailableCharacters() const
{
	FString FinalText = "AVAILABLE CHARACTERS: ";
	int i = 0;

	for (const USevenCharacterDA* Seven : AvailableCharacters)
	{
		const FString Name = Seven->Name.ToString() + " " + Seven->Description.ToString();
		FinalText += "SevenCharacter: " + FString::FromInt(i++) + " " + Name + ".....";
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FinalText);
	}
}

void ACharacterPicker::ShowSelectedCharacters() const
{
	FString FinalText = "SELECTED CHARACTERS: ";
	int i = 0;

	UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	const TArray<USevenCharacterDA*>& SelectedCharacters = GameController->SelectedCharacters;

	for (const USevenCharacterDA* Seven : SelectedCharacters)
	{
		const FString Name = Seven->Name.ToString();
		FinalText += FString::FromInt(i++) + Name + ".....";
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FinalText);
	}
}

void ACharacterPicker::SelectCharacters(TArray<int> SelectedSlots)
{
	SelectedSlots.Sort();

	UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());

	if (AvailableCharacters.Num() < SelectedSlots[SelectedSlots.Num() - 1])
	{
		UE_LOG(LogTemp, Error, TEXT("[ACharacterPicker].SelectCharacters SelectedSlots is higher than amount of Available Charactes"));
		return;
	}

	for (int& SelectedSlot : SelectedSlots)
	{
		GameController->SelectedCharacters.Add(AvailableCharacters[SelectedSlot]);
	}
}

void ACharacterPicker::SpawnSevenCharacters() const
{
	UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	const TArray<USevenCharacterDA*>& SelectedCharacters = GameController->SelectedCharacters;

	UE_LOG(LogTemp, Display, TEXT("[ACharacterPicker].SpawnSevenCharacters, amount: %d"), SelectedCharacters.Num());

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (USevenCharacterDA* CharacterToBeSpawned : SelectedCharacters)
	{
		if (!CharacterToBeSpawned->RepresentingClass)
		{
			UE_LOG(LogTemp, Error, TEXT("[ACharacterPicker].SpawnSevenCharacters Unable to spawn %s"), *CharacterToBeSpawned->Name.ToString());
			continue;
		}

		FTransform T{ GetActorLocation() };
		ASevenCharacter* SevenCharacter = GetWorld()->SpawnActorDeferred<ASevenCharacter>(
			CharacterToBeSpawned->RepresentingClass,
			T,
			nullptr,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
		);

		SevenCharacter->SevenCharacterDA = CharacterToBeSpawned; 
		SevenCharacter->FinishSpawning(T);
	}
}


