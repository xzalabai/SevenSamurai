#include "VV_WeaponUpgrade.h"
#include "GameController.h"
#include "SevenCharacterDA.h"

void AVV_WeaponUpgrade::OnOverlapAction()
{
	// UI Should pop out!

	// Now we just have to fake everything :(
	UE_LOG(LogTemp, Display, TEXT("[AVV_WeaponUpgrade].OnOverlapAction Upgrading Weapon"));

	const UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	const TArray<USevenCharacterDA*> SevenCharacter = GameController->GetSelectedCharacters();
	SevenCharacter[0]->WeaponDetail = FWeaponDetail(4000, EWeaponLevel::Two);
}
