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

DECLARE_MULTICAST_DELEGATE(FOnRestart);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatusUpdate, const AActor* Actor, const EEnemyStatus Status);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnMissionUpdate, const UMissionDA* Mission, const EMissionStatus Status);

static uint8 UniqueIDCounter;

UCLASS()
class SEVEN_API UGameController : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class ASevenPlayerController;

public:
	FOnRestart OnRestart;
	FOnStatusUpdate OnStatusUpdate;
	FOnMissionUpdate OnMissionUpdate;
	UPROPERTY()
	TArray<USevenCharacterDA*> SelectedCharacters{};

private:
	UPROPERTY()
	TMap<int8, EEnemyStatus> EnemiesStatus;
	
	TMap<int8, EEnemyStatus> SevenCharactersStatus;
	
	UPROPERTY()
	TArray<const ASevenCharacter*> SevenCharacters;
	
	UPROPERTY()
	TArray<const ASevenCharacter*> Enemies;

	UPROPERTY()
	mutable const UMissionDA* ActiveMission;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	const EEnemyStatus GetEnemyStatus(const int8 CharacterID) const;
	bool HasAnyEnemyStatus(const EEnemyStatus& Status) const;
	void OnCharacterKilled(const AActor* Actor, const EEnemyStatus Status);
	ASevenCharacter* GetAnyAliveEnemy();
	ASevenCharacter* GetPossessedCharacter() const;
	ASevenPlayerController* GetSevenPlayerController() const;
	TArray<const ASevenCharacter*> GetAIControlledAllies() const;
	const TArray<const ASevenCharacter*> GetSevenCharacters() const;
	void SetActiveMission(const UMissionDA* Mission);
	void UpdateMissionParameters(AMission* Mission);
	UFUNCTION(BlueprintCallable)
	void AddToSelectedCharacter(USevenCharacterDA* SevenCharacterDA);
	void MissionEnd(bool bWin);
	void UpdateSevenCharacters();
	UFUNCTION(BlueprintCallable)
	const TArray<USevenCharacterDA*> GetSelectedCharacters() const;
public:
	UFUNCTION()
	void Restart();

	void UpdateStatus(const AActor* Actor, const EEnemyStatus Status = EEnemyStatus::None);

};
