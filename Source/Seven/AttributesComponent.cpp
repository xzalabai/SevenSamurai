#include "AttributesComponent.h"
#include "Math/UnrealMathUtility.h"
#include "SevenCharacterDA.h"
#include "SevenPlayerController.h"
#include "SevenCharacter.h"
#include "AnimationComponent.h"

UAttributesComponent::UAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	AttributesMap.Add(EItemType::HP, 0);
	AttributesMap.Add(EItemType::XP, 0);

	MaxAttributesMap.Add(EItemType::XP, 3);
	MaxAttributesMap.Add(EItemType::HP, 3);

}


void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();

	const ASevenCharacter* const SevenCharacter = GetOwnerCharacter();
	bIsPlayer = SevenCharacter->CanBePossessed();
	if (bIsPlayer)
	{
		const USevenCharacterDA* const SevenCharacterDA = SevenCharacter->SevenCharacterDA;
		check(SevenCharacterDA->HP != 0);
		Set(EItemType::HP, SevenCharacterDA->HP);
		Set(EItemType::XP, 0);
		SevenPlayerController = GetOwnerCharacter()->GetSevenPlayerController();
	}
	else
	{
		Set(EItemType::HP, 3);
		Set(EItemType::XP, 100);
	}
}

void UAttributesComponent::Set(const EItemType ItemType, const float Amount)
{
	AttributesMap[ItemType] = Amount;
	NotifyUI(ItemType);
}

void UAttributesComponent::Add(const EItemType ItemType, const float Amount)
{
	UE_LOG(LogTemp, Error, TEXT("[UAttributesComponent].Add %d %d"), (int)ItemType, Amount);

	AttributesMap[ItemType] = FMath::Min(AttributesMap[ItemType] + Amount, MaxAttributesMap[ItemType]);;
	NotifyUI(ItemType);
}

void UAttributesComponent::NotifyUI(const EItemType ChangedItemType) const
{
	if (!bIsPlayer || !SevenPlayerController)
	{
		// Don't update for Enemy
		// SevenPlayerController might be nullptr in the beginning
		return;
	}

	SevenPlayerController->UpdateUI(ChangedItemType, AttributesMap[ChangedItemType]);
}

void UAttributesComponent::OnPossessed() const
{
	if (!bIsPlayer)
	{
		return;
	}

	for (const auto& ItemType : AttributesMap)
	{
		SevenPlayerController->UpdateUI(ItemType.Key, ItemType.Value);
	}
}

uint16 UAttributesComponent::Decrease(const EItemType ItemType, const float Decrease)
{
	AttributesMap[ItemType] = FMath::Max(0, AttributesMap[ItemType] - Decrease);
	NotifyUI(ItemType);

	return AttributesMap[ItemType];
}

ASevenCharacter* UAttributesComponent::GetOwnerCharacter()
{
	if (ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(GetOwner()))
	{
		return SevenCharacter;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UAnimationComponent] SevenCharacter Not found"));
		return nullptr;
	}
}

	