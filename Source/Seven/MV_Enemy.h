#pragma once

#include "CoreMinimal.h"
#include "MV_EntityBase.h"
#include "MV_Enemy.generated.h"

class UFloatingPawnMovement;
class UPaperSprite;
class UMissionDA;


UCLASS()
class SEVEN_API AMV_Enemy : public AMV_EntityBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UFloatingPawnMovement* MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Sprite,Rendering,Physics,Components|Sprite", AllowPrivateAccess = "true"))
	TObjectPtr<UPaperSprite> EnemyCampImage;

public:
	AMV_Enemy();
	virtual void OnPossessed() override;
	virtual void Camp(const bool bEnable);

protected:
	virtual void BeginPlay() override;
};
