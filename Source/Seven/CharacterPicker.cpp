#include "CharacterPicker.h"
#include "SevenCharacter.h"
#include "SevenCharacterDA.h"
#include "Weapon.h"
#include "WeaponUpgrade.h"
#include "SevenPlayerController.h"
#include "MissionHandler.h"
#include "Kismet\GameplayStatics.h"

ACharacterPicker::ACharacterPicker()
{
	PrimaryActorTick.bCanEverTick = false;

}

void ACharacterPicker::BeginPlay()
{
	Super::BeginPlay();
	AMissionHandler* MissionHandler = Cast<AMissionHandler>(UGameplayStatics::GetActorOfClass(this, AMissionHandler::StaticClass()));
	MissionHandler->OnMissionEnd.AddUObject(this, &ACharacterPicker::OnMissionEnd);

	AWeaponUpgrade* WeaponUpgrade = Cast<AWeaponUpgrade>(UGameplayStatics::GetActorOfClass(this, AWeaponUpgrade::StaticClass()));
	WeaponUpgrade->OnWeaponUpgrade.AddUObject(this, &ACharacterPicker::OnWeaponUpgrade);
}

void ACharacterPicker::OnMissionEnd(bool bPlayerWon)
{
	// Update existing entries
	TObjectPtr<ASevenPlayerController> SevenPlayerController = Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	const TArray<const ASevenCharacter*> AllSevenCharacters = SevenPlayerController->GetSevenCharacters();
	
	for (const ASevenCharacter* SevenCharacter : AllSevenCharacters)
	{
		if (!SevenCharacter->IsAlive())
		{
			AvailableCharacters.RemoveSwap(SevenCharacter->SevenCharacterDA);
		}
	}
}

void ACharacterPicker::OnWeaponUpgrade(const AActor* UpgradedActor)
{
	// No need to update anything here, since Update of Weapon for SevenCharacterDA happens via pointers in AttackComponent/SetWeaponDamage

	/*const ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(UpgradedActor);
	int32 Index = AvailableCharacters.Find(SevenCharacter->SevenCharacterDA);
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogTemp, Error, TEXT("[ACharacterPicker].UpgradedActor NULL"));
		return;
	}
	USevenCharacterDA* SevenCharacterDA = AvailableCharacters[Index];*/
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

	for (USevenCharacterDA* CharacterToBeSpawned : SelectedCharacters)
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
		SevenCharacter->SevenCharacterDA = CharacterToBeSpawned; 

	}
}


