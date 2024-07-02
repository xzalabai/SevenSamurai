#pragma once

#include "CoreMinimal.h"
#include "VV_EntityBase.h"
#include "ComboManager.h"
#include "VV_CharacterUpgrade.generated.h"

UCLASS()
class SEVEN_API AVV_CharacterUpgrade : public AVV_EntityBase
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
	TMap<ESevenCharacterType, FSevenCharacterCombos> SevenCharacterCombosMapping;

public:
	virtual void OnOverlapAction() override;
};
