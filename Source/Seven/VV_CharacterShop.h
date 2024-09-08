#pragma once

#include "CoreMinimal.h"
#include "VV_EntityBase.h"
#include "VV_CharacterShop.generated.h"

class USevenCharacterDA;


UCLASS()
class SEVEN_API AVV_CharacterShop : public AVV_EntityBase
{
	GENERATED_BODY()

	friend class AVV_Map;

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<USevenCharacterDA*> AllCharacters;

	UPROPERTY()
	TArray<USevenCharacterDA*> AvailableCharacters{};

public:
	virtual void OnOverlapAction() override;
	void PickCharacter(const int Index);
	virtual void BeginPlay() override;
	const TArray<USevenCharacterDA*>& GenerateAvailableCharacters();
};
