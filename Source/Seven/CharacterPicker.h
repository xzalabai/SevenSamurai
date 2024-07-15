#pragma once

#include "CoreMinimal.h"
#include "PublicEnums.h"
#include "GameFramework/Actor.h"
#include "CharacterPicker.generated.h"

class USevenCharacterDA;
class ASevenCharacter;

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

private:	
	UFUNCTION(BlueprintCallable)
	void ShowAvailableCharacters() const;

	UFUNCTION(BlueprintCallable)
	void ShowSelectedCharacters() const;

	UFUNCTION(BlueprintCallable)
	void SelectCharacters(TArray<int32> SelectedSlots);

	UFUNCTION(BlueprintCallable)
	void SpawnSevenCharacters() const;

};
