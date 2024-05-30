#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AISevenCharacterController.generated.h"

UCLASS()
class SEVEN_API AAISevenCharacterController : public AAIController
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	class UBehaviorTree* AIBehavior;
};
