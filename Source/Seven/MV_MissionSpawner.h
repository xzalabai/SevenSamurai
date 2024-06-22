#pragma once

#include "CoreMinimal.h"
#include "MissionDA.h"
#include "GameFramework/Actor.h"
#include "MV_MissionSpawner.generated.h"

class UMissionDA;
class UPaperSpriteComponent;
class AMV_Map;
class AMV_EntityBase;

UCLASS()
class SEVEN_API AMV_MissionSpawner : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	TArray<UMissionDA*> AvailableMissions;

	UPROPERTY()
	AMV_Map* Map;

	UPROPERTY(EditDefaultsOnly)
	TArray<AMV_EntityBase*> DisplayedMissions;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMV_EntityBase> MissionClass;

protected:
	virtual void BeginPlay() override;

public:	
	AMV_MissionSpawner();
	virtual void Tick(float DeltaTime) override;
	FVector GetRandomPointOnMap(UPaperSpriteComponent* SpriteComponent) const;
	void OnMissionUpdate(const UMissionDA* Mission, const EMissionStatus Status);

};
