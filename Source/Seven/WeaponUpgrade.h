#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponUpgrade.generated.h"

class ASevenCharacter;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnWeaponUpgrade, const AActor* UpgradedActor);

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

	FOnWeaponUpgrade OnWeaponUpgrade;

protected:
	virtual void BeginPlay() override;
};
