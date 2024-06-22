#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PublicEnums.h"
#include "MissionHandler.generated.h"

class AMission;
class AEnemyCharacter;
class UEnemyScenarios;

UCLASS()
class SEVEN_API AMissionHandler : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere)
	TMap<uint32, AMission*> Missions;

	UPROPERTY(VisibleAnywhere)
	int32 ActiveMissionID{ -1 };

	UPROPERTY(VisibleAnywhere)
	uint32 EnemyKilledCount{ 0 };
	
	UPROPERTY(VisibleAnywhere)
	uint32 SevenCharactersKilledCount{ 0 };

public:	
	AMissionHandler();


protected:
	virtual void BeginPlay() override;
	TSubclassOf<AEnemyCharacter> EnemyClassToSpawn;
};
