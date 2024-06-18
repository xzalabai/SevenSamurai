#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PublicEnums.h"
#include "GameController.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnRestart);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatusUpdate, const AActor* Actor, const EEnemyStatus Status);

class ASevenCharacter;
class ASevenPlayerController;

static uint8 UniqueIDCounter;

UCLASS()
class SEVEN_API UGameController : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	friend class ASevenPlayerController;

public:
	FOnRestart OnRestart;
	FOnStatusUpdate OnStatusUpdate;

private:
	UPROPERTY()
	TMap<int8, EEnemyStatus> EnemiesStatus;
	
	UPROPERTY()
	TMap<int8, EEnemyStatus> SevenCharactersStatus;
	
	UPROPERTY()
	TArray<const ASevenCharacter*> SevenCharacters;
	
	UPROPERTY()
	TArray<const ASevenCharacter*> Enemies;

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

public:
	UFUNCTION()
	void Restart();

	void UpdateStatus(const AActor* Actor, const EEnemyStatus Status = EEnemyStatus::None);

};
