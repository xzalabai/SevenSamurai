#pragma once
#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "MissionDA.h"
#include "MV_Map.generated.h"

class UMissionDA;
class UPaperSpriteComponent;
class AMV_EntityBase;
class AMV_Enemy;
class AMVSevenCharacter;

UCLASS()

class SEVEN_API AMV_Map : public APaperSpriteActor
{
	friend class UGameController;

	GENERATED_BODY()

private:	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	TArray<UMissionDA*> AvailableMissions;

	UPROPERTY();
	TArray<const AMV_EntityBase*> ActiveEntities;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMV_EntityBase> MissionClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMV_Enemy> EnemyClass;

	UPROPERTY()
	TObjectPtr<AMVSevenCharacter> MVSevenCharacter;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	FVector GetRandomPointOnMap() const;
	TObjectPtr<AMVSevenCharacter> GetMVSevenCharacter() const;
	
};
