#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponUpgrade.generated.h"

class ASevenCharacter;

UCLASS()
class SEVEN_API AWeaponUpgrade : public AActor
{
	GENERATED_BODY()
	
public:	
	AWeaponUpgrade();

	UFUNCTION(BlueprintCallable)
	void ShowPossibleWeaponUpgrades() const;

	UFUNCTION(BlueprintCallable)
	void UpgradeWeapon(const ASevenCharacter* Character);

protected:
	virtual void BeginPlay() override;
};
