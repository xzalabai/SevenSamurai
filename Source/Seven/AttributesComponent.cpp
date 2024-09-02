#include "AttributesComponent.h"
#include "Math/UnrealMathUtility.h"
#include "SevenCharacterDA.h"
#include "SevenCharacter.h"
#include "AnimationComponent.h"

UAttributesComponent::UAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
	AttributesMap.Add(EItemType::HP, 0);
	AttributesMap.Add(EItemType::XP, 0);

	MaxAttributesMap.Add(EItemType::XP, 100);
	MaxAttributesMap.Add(EItemType::HP, 100);
		
	if (const USevenCharacterDA* SevenCharacterDA = GetOwnerCharacter()->SevenCharacterDA)
	{
		check(SevenCharacterDA->HP != 0);
		Set(EItemType::HP, SevenCharacterDA->HP);
		UE_LOG(LogTemp, Error, TEXT("[UAnimationComponent] HP OF CHARACTER %s is %d"), *SevenCharacterDA->Name.ToString(), SevenCharacterDA->HP);
	}
	else
	{
		// Enemy .. for now keep it 100
		Set(EItemType::HP, 100);
	}
	
}

void UAttributesComponent::Set(const EItemType ItemType, const uint16 Amount)
{
	AttributesMap[ItemType] = Amount;
}

void UAttributesComponent::Add(const EItemType ItemType, const uint16 Amount)
{
	UE_LOG(LogTemp, Error, TEXT("[UAttributesComponent].Add %d %d"), (int)ItemType, Amount);
	const uint16 ValueToAdd = AttributesMap[ItemType] + Amount;
	AttributesMap[ItemType] = FMath::Min(ValueToAdd, MaxAttributesMap[ItemType]);;
}

const uint16& UAttributesComponent::Decrease(const EItemType ItemType, const int32 Decrease)
{
	AttributesMap[ItemType] = FMath::Max(0, AttributesMap[ItemType] - Decrease);
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

	