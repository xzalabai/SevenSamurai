#pragma once

#include "CoreMinimal.h"
#include "MV_EntityBase.h"
#include "MV_Enemy.generated.h"

class UFloatingPawnMovement;

UCLASS()
class SEVEN_API AMV_Enemy : public AMV_EntityBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UFloatingPawnMovement* MovementComponent;

public:
	AMV_Enemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
