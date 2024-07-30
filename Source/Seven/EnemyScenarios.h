#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Mission.h"
#include "PublicEnums.h"
#include "BehaviorTree/BehaviorTree.h"
#include "EnemyScenarios.generated.h"

UCLASS()
class SEVEN_API UEnemyScenarios : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TMap<ESevenCharacterType, UBehaviorTree*> EnemyScenarios;
};
