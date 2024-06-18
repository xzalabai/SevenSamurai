#include "WeaponUpgrade.h"
#include "SevenCharacter.h"
#include "AttackComponent.h"
#include "Kismet\GameplayStatics.h"
#include "GameController.h"

AWeaponUpgrade::AWeaponUpgrade()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWeaponUpgrade::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeaponUpgrade::ShowPossibleWeaponUpgrades() const
{
	const UGameInstance* GameInstance = Cast<UGameInstance>(GetWorld()->GetGameInstance());
	UGameController* GameController = Cast<UGameController>(GameInstance->GetSubsystem<UGameController>());

	const TArray<const ASevenCharacter*> SevenCharacters = GameController->GetSevenCharacters();

	FString FinalText = "";
	for (const ASevenCharacter* SevenCharacter : SevenCharacters)
	{
		FString CurrentWeaponDamage = FString::FromInt(SevenCharacter->AC_AttackComponent->GetWeaponDamage());
		FinalText += "SevenCharacter: " + FString::FromInt(SevenCharacter->GetUniqueID()) + " has Weapon Damage: " + CurrentWeaponDamage + ".....";
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FinalText);
	}
}

void AWeaponUpgrade::UpgradeWeapon(const ASevenCharacter* SevenCharacter)
{
	// upgrade double..
	//SevenCharacter->AC_AttackComponent->SetWeaponDamage(SevenCharacter->AC_AttackComponent->GetWeaponDamage() * 2);
	SevenCharacter->AC_AttackComponent->SetWeaponDamage(888);

	OnWeaponUpgrade.Broadcast(SevenCharacter);
}
