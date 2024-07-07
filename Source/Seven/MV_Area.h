#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MV_OverlapInterface.h"
#include "MV_Area.generated.h"

class UBoxComponent;
class AMV_Map;

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
	
public:	
	AMV_Area();

protected:
	virtual void BeginPlay() override;
public:
	virtual void OnOverlapAction() override;

};
