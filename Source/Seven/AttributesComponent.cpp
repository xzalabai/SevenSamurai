#include "AttributesComponent.h"
#include "Math/UnrealMathUtility.h"

UAttributesComponent::UAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();

	for (int i = static_cast<int>(EItemType::Gold); i <= static_cast<int>(EItemType::Rice); ++i) {
		EItemType ItemType = static_cast<EItemType>(i);
		AttributesMap.Add(ItemType, 0);
	}
	AttributesMap[EItemType::HP] = 150;
}

void UAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributesComponent::Set(const EItemType ItemType, const int32 NewHP)
{
	AttributesMap[ItemType] = NewHP;
}

void UAttributesComponent::Add(const EItemType ItemType, const int32 NewHP)
{
	AttributesMap[ItemType] = AttributesMap[ItemType] + NewHP;
}

const int32& UAttributesComponent::Decrease(const EItemType ItemType, const int32 Decrease)
{
	AttributesMap[ItemType] = FMath::Max(0, AttributesMap[ItemType] - Decrease);
	return AttributesMap[ItemType];
}

	