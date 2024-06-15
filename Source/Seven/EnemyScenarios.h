#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Mission.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyScenarios.generated.h"


USTRUCT()
struct FEnemyScenario {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EMissionType MissionType;

	UPROPERTY(EditAnywhere)
	UBehaviorTree* BTAsset;
};

UCLASS()
class SEVEN_API UEnemyScenarios : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TMap<EMissionType, UBehaviorTree*> EnemyScenarios;
};
