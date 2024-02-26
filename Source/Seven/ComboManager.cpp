#include "ComboManager.h"
#include "Combo.h"

UComboManager::UComboManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UComboManager::BeginPlay()
{
	Super::BeginPlay();

	// Add listed combos into mappings
	for (int i = 0; i < Combos.Num(); ++i)
	{
		CombosMapping.Emplace(i+1, NewObject<UCombo>(this, Combos[i]));
	}
}

void UComboManager::UseCombo(const ESpecial& Special)
{
	UE_LOG(LogTemp, Display, TEXT("[UComboManager].UseCombo"));
	
	if (Combos.Num() == 0)
	{
		UE_LOG(LogTemp, Display, TEXT("[UComboManager].UseCombo is Empty"));
		return;
	}

	if (Special == ESpecial::ES_Special1 && CombosMapping.Contains(1))
	{
		UE_LOG(LogTemp, Display, TEXT("[UComboManager].UseCombo number 1"));
		UCombo* Combo = CombosMapping[1];
		Combo->Use(GetOwner(), nullptr);
	}
}
