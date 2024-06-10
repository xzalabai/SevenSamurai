#include "ComboManager.h"
#include "AttributesComponent.h"
#include "Combo.h"
#include "AttackComponent.h"
#include "SevenCharacterDA.h"
#include "SevenCharacter.h"
#include "RadialCombo.h"

AComboManager::AComboManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AComboManager::ShowSkillTree(const TArray<ASevenCharacter*> AvailableCharacters) const
{
	for (const ASevenCharacter* AvailableCharacter : AvailableCharacters)
	{
		
		if (!SevenCharacterCombosMapping.Contains(AvailableCharacter->GetSevenCharacterType()))
		{
			UE_LOG(LogTemp, Error, TEXT("[AComboManager]ShowSkillTree Wrong mapping for SevenCharacterType: %d"), (int)AvailableCharacter->GetSevenCharacterType());
			return;
		}

		const FSevenCharacterCombos& SevenCharacterCombos = SevenCharacterCombosMapping[AvailableCharacter->GetSevenCharacterType()];
		FString CombosInText = "";
		
		for (const FCombosWithPrices& Combos : SevenCharacterCombos.CombosAvailable)
		{
			FString PriceString = FString::FromInt(Combos.Price);
			CombosInText += Combos.Combo->GetName() + " = " + PriceString + "$ ..... ";
		}

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, CombosInText);
		}
	}
}

void AComboManager::BuyCombo(const ASevenCharacter* const SevenCharacter, const int Index) const
{
	if (!SevenCharacterCombosMapping.Contains(SevenCharacter->GetSevenCharacterType()))
	{
		UE_LOG(LogTemp, Error, TEXT("[AComboManager]ShowSkillTree Wrong mapping for SevenCharacterType: %d"), (int)SevenCharacter->GetSevenCharacterType());
		return;
	}
	const FSevenCharacterCombos SevenCharacterCombos = SevenCharacterCombosMapping[SevenCharacter->GetSevenCharacterType()];
	const FCombosWithPrices ComboToBePurchased = SevenCharacterCombos.CombosAvailable[Index];

	UAttributesComponent* AttributesComponent = SevenCharacter->AC_Attribute;
	UAttackComponent* AttackComponent = SevenCharacter->AC_AttackComponent;

	if (!AttributesComponent || !AttackComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("[AComboManager]BuyCombo AttributesComponent or AttackComponent is Nullptr"));
		return;
	}

	if (AttributesComponent->GetGold() - ComboToBePurchased.Price < 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AComboManager]BuyCombo Not Enough Gold. Current amount: %d"), AttributesComponent->GetGold()); 
		return;
	}

	AttributesComponent->Decrease(EItemType::Gold, ComboToBePurchased.Price);
	AttackComponent->AddComboToCharacter(ComboToBePurchased.Combo);

}

