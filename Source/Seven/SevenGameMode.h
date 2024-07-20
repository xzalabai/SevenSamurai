// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PublicEnums.h"
#include "Mission.h"
#include "SevenGameMode.generated.h"

class ULootGenerator;
class AMission;
class ASevenCharacter;
class ASevenPlayerController;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStatusUpdate, const AActor* Actor, const EEnemyStatus Status);

UCLASS(minimalapi)
class ASevenGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<const ASevenCharacter*> SevenCharacters;

	UPROPERTY()
	TMap<int8, EEnemyStatus> EnemiesStatus;

	UPROPERTY()
	TMap<int8, EEnemyStatus> SevenCharactersStatus;

	UPROPERTY()
	TArray<const ASevenCharacter*> Enemies;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASevenCharacter> SevenToSpawn; // TODO Remove for final - this is only for DEBUG! when u start game in combat 

	UPROPERTY(EditAnywhere) // TODO Remove for final - this is only for DEBUG! when u start game in combat 
	EMissionType MissionType;

	UPROPERTY(EditAnywhere) // TODO Remove for final - this is only for DEBUG! when u start game in combat 
	TMap<int, TSubclassOf<AEnemyCharacter>> EnemiesToSpawn;

	bool bIsDebugBattle{ false };

public:
	FOnStatusUpdate OnStatusUpdate; // Updates about all characters (Sevens and Enemies)
	FOnStatusUpdate OnSevenCharacterStatusUpdate; // Update only about player

public:
	ASevenGameMode();
	
	UPROPERTY(EditDefaultsOnly)
	ULootGenerator* AC_LootGenerator;

public:
	const TArray<const ASevenCharacter*> GetSevenCharacters() const;
	void UpdateStatus(const AActor* Actor, const EEnemyStatus Status = EEnemyStatus::None);
	ASevenCharacter* GetAnyAliveEnemy();
	ASevenCharacter* GetPossessedCharacter() const;
	ASevenPlayerController* GetSevenPlayerController() const;
	TMap<int8, EEnemyStatus> GetSevenCharactersStatus() const;
	TArray<const ASevenCharacter*> GetAIControlledAllies() const;
	bool HasAnyEnemyStatus(const EEnemyStatus& Status) const;
	const EEnemyStatus GetEnemyStatus(const int8 CharacterID) const;
	void MissionEnd(bool bWin);
	void UpdateMissionParameters(AMission* Mission);
};



