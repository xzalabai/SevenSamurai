#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ComboManager.generated.h"

class ACombo;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEVEN_API UComboManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UComboManager();
	void UseCombo(const FName& Name);

protected:
	virtual void BeginPlay() override;
	
////////////////////////////////////////////////////////////////////////////
// Variables
protected:
	//UPROPERTY(EditDefaultsOnly)
	//TArray<ACombo*> Combos;
	//UPROPERTY(EditAnywhere)
	//TSubclassOf<ACombo> Combo;
};
