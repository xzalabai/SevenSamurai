#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MV_OverlapInterface.h"
#include "MV_Area.generated.h"

class UBoxComponent;
class AMV_Map;

UENUM(BlueprintType)
enum class EAreaStatus : uint8
{
	OccupiedByEnemies,
	Liberated,
};


UCLASS()
class SEVEN_API AMV_Area : public AActor, public IMV_OverlapInterface
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int8 UniqueAreaID;

	UPROPERTY(EditAnywhere)
	UBoxComponent* BoxComponent;

	UPROPERTY()
	const AMV_Map* Map;
	
	UPROPERTY()
	int8 ActiveEnemiesInArea{ 0 };

	UPROPERTY()
	int8 TotalEnemiesInArea{ 0 }; // In total (in the beginning)

	UPROPERTY()
	EAreaStatus AreaStatus{ EAreaStatus::OccupiedByEnemies };

public:	
	AMV_Area();

protected:
	virtual void BeginPlay() override;
public:
	virtual void OnOverlapAction() override;

};
