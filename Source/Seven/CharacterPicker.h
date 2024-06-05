#pragma once

#include "CoreMinimal.h"
#include "PublicEnums.h"
#include "GameFramework/Actor.h"
#include "CharacterPicker.generated.h"

class USevenCharacterDA;
class ASevenCharacter;
//class ESevenCharacterType;

UCLASS()
class SEVEN_API ACharacterPicker : public AActor
{
	GENERATED_BODY()
	
public:	
	ACharacterPicker();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	TArray<USevenCharacterDA*> AvailableCharacters;
	
	UPROPERTY()
	TArray<USevenCharacterDA*> SelectedCharacters;

private:
	//UFUNCTION(BlueprintCallable)
	
	// Listeners
	void OnMissionEnd(bool bPlayerWon);
	void OnWeaponUpgrade(const AActor* UpgradedActor);

	UFUNCTION(BlueprintCallable)
	void ShowAvailableCharacters() const;

	UFUNCTION(BlueprintCallable)
	void ShowSelecteCharacters() const;

	UFUNCTION(BlueprintCallable)
	void SelectCharacters(TArray<int32> SelectedSlots);

	UFUNCTION(BlueprintCallable)
	void SpawnSevenCharacters() const;

};
