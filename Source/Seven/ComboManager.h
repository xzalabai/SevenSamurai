#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PublicEnums.h"
#include "ComboManager.generated.h"

class UCombo;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEVEN_API UComboManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UComboManager();
	void UseCombo(const ESpecial& Special);

protected:
	virtual void BeginPlay() override;
	
////////////////////////////////////////////////////////////////////////////
// Variables
public:
	UPROPERTY(VisibleAnywhere)
	ESpecial SpecialActivated = ESpecial::ES_None;

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UCombo>> Combos;
	
	UPROPERTY()
	TMap<int, UCombo*> CombosMapping;
};
