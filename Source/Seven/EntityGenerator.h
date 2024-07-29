#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Mission.h"
#include "MissionDA.h"
#include "EntityGenerator.generated.h"

class UDataTable;
class UGameController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEVEN_API UEntityGenerator : public UActorComponent
{
	friend class AMV_Map;

	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDataTable> MissionsDT;

public:	
	UEntityGenerator();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	UMissionDA* GenerateMission(const uint8 AreaIndex, EMissionType MissionType = EMissionType::NotProvided) const;
	void ConvertMissionToMissionDA(const FUMissionDT* MissionDT, UMissionDA* MissionDA) const;
	UGameController* GameController = nullptr;
};
