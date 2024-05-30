#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArmorUpgrade.generated.h"

class ASevenCharacter;

UCLASS()
class SEVEN_API AArmorUpgrade : public AActor
{
	GENERATED_BODY()
	
public:	
	AArmorUpgrade();

protected:
	virtual void BeginPlay() override;

	//UFUNCTION(BlueprintCallable)
	//void ShowPossibleArmorUpgrades();

	//UFUNCTION(BlueprintCallable)
	//void UpgradeArmor(const ASevenCharacter* Character);
};
