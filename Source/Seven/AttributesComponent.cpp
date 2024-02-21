#include "AttributesComponent.h"

UAttributesComponent::UAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
}
