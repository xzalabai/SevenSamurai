#include "CharacterPicker.h"
#include "SevenCharacter.h"
#include "SevenCharacterDA.h"

ACharacterPicker::ACharacterPicker()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ACharacterPicker::BeginPlay()
{
	Super::BeginPlay();
	
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

void ACharacterPicker::ShowSelecteCharacters() const
{
	FString FinalText = "SELECTED CHARACTERS: ";
	int i = 0;

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
	SelectedCharacters.Empty();

	if (AvailableCharacters.Num() < SelectedSlots[SelectedSlots.Num() - 1])
	{
		UE_LOG(LogTemp, Error, TEXT("[ACharacterPicker].SelectCharacters SelectedSlots is higher than amount of Available Charactes"));
		return;
	}

	for (int& SelectedSlot : SelectedSlots)
	{
		SelectedCharacters.Add(AvailableCharacters[SelectedSlot]);
	}
}

void ACharacterPicker::SpawnSevenCharacters() const
{
	UE_LOG(LogTemp, Display, TEXT("[ACharacterPicker].SpawnSevenCharacters, amount: %d"), SelectedCharacters.Num());
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (const USevenCharacterDA* CharacterToBeSpawned : SelectedCharacters)
	{
		if (!CharacterToBeSpawned->RepresentingClass)
		{
			UE_LOG(LogTemp, Error, TEXT("[ACharacterPicker].SpawnSevenCharacters Unable to spawn %s"), *CharacterToBeSpawned->Name.ToString());
			continue;
		}
		ASevenCharacter* SevenCharacter = GetWorld()->SpawnActor<ASevenCharacter>(
			CharacterToBeSpawned->RepresentingClass,
			GetActorLocation(),
			FRotator(),
			SpawnParams);
	}
}


