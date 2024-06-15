#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PublicEnums.h"
#include "MissionHandler.generated.h"

class AMission;
class AEnemyCharacter;
class UEnemyScenarios;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMissionEnd, bool bPlayerWon);

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

	UFUNCTION(BlueprintCallable)
	void StoreMissions();

	FOnMissionEnd OnMissionEnd;

protected:
	virtual void BeginPlay() override;
	void MissionStarted(uint32 ID);
	UFUNCTION(BlueprintCallable)
	void MovingToMissionArea();
	void MoveAlliesToPlace();
	void OnStatusUpdate(const AActor* Actor, const EEnemyStatus Status);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEnemyCharacter> EnemyClassToSpawn;
};
