#pragma once
#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "MissionDA.h"
#include "MV_Map.generated.h"

class UMissionDA;
class UPaperSpriteComponent;
class AMV_EntityBase;

UCLASS()

class SEVEN_API AMV_Map : public APaperSpriteActor
{
	GENERATED_BODY()

private:	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	TArray<UMissionDA*> AvailableMissions;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMV_EntityBase> MissionClass;


protected:
	virtual void BeginPlay() override;

private:
	FVector GetRandomPointOnMap(UPaperSpriteComponent* SpriteComponent) const;
	
};
