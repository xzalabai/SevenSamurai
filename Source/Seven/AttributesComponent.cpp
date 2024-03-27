#include "AttributesComponent.h"
#include "Math/UnrealMathUtility.h"

UAttributesComponent::UAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
	AttributesMap.Add(EItemType::Gold,0);
	AttributesMap.Add(EItemType::HP, 100);
	AttributesMap.Add(EItemType::XP, 0);
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

	