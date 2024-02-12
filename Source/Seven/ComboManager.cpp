#include "ComboManager.h"
#include "Combo.h"

UComboManager::UComboManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UComboManager::BeginPlay()
{
	Super::BeginPlay();
}

void UComboManager::UseCombo(const FName& Name)
{
	//if (Combos.Num() == 0)
	//{
	//	UE_LOG(LogTemp, Display, TEXT("[UComboManager].UseCombo is Empty"));
	//	return;
	//}
	//Combos[0]->Use(GetOwner(), nullptr);
}
