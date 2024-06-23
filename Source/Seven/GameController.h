#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PublicEnums.h"
#include "MissionDA.h"
#include "GameController.generated.h"

class ASevenCharacter;
class USevenCharacterDA;
class ASevenPlayerController;
class AMission;

static uint8 UniqueIDCounter;

UCLASS()
class SEVEN_API UGameController : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class ASevenGameMode;

public:
	UPROPERTY()
	TArray<USevenCharacterDA*> SelectedCharacters{};

private:
	UPROPERTY()
	mutable const UMissionDA* ActiveMission;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void SetActiveMission(const UMissionDA* Mission);
	UFUNCTION(BlueprintCallable)
	void AddToSelectedCharacter(USevenCharacterDA* SevenCharacterDA);
	UFUNCTION(BlueprintCallable)
	const TArray<USevenCharacterDA*> GetSelectedCharacters() const;
};
