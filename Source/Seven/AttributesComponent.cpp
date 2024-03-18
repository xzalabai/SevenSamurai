#include "AttributesComponent.h"
#include "Math/UnrealMathUtility.h"

UAttributesComponent::UAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

const int32& UAttributesComponent::DecreaseHP(const int32 Decrease)
{
	HP = FMath::Max(0, HP - Decrease);
	return HP;
}

void UAttributesComponent::SetHP(const int32 NewHP)
{
	HP = NewHP;
}

