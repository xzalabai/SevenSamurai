#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "VV_Map.generated.h"

class AMVSevenCharacter;

UCLASS()
class SEVEN_API AVV_Map : public APaperSpriteActor
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AMVSevenCharacter> MVSevenCharacter;

public:
	TObjectPtr<AMVSevenCharacter> GetMVSevenCharacter() const;
	virtual void BeginPlay() override;
	
};
