#pragma once

#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "MV_OverlapInterface.h"
#include "VV_Map.generated.h"

class AMVSevenCharacter;
class UCapsuleComponent;

UCLASS()
class SEVEN_API AVV_Map : public APaperSpriteActor, public IMV_OverlapInterface
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AMVSevenCharacter> MVSevenCharacter;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	UCapsuleComponent* CapsuleComponent;

	int VillageID;

public:
	AVV_Map();
	const TObjectPtr<AMVSevenCharacter> GetMVSevenCharacter() const;
	virtual void BeginPlay() override;
protected:
	virtual void OnOverlapAction() override;
private:
	void ExitVillage() const;
	
};
