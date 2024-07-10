#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PublicEnums.h"
#include "Templates/UniquePtr.h"
#include "MissionDA.h"
#include "GameController.generated.h"

class ASevenCharacter;
class USevenCharacterDA;
class ASevenPlayerController;
class AMV_Map;
class UQuest;
class AMV_QuestGiver;
class AMission;
class AMV_EntityBase;

static uint8 UniqueIDCounter;

USTRUCT(BlueprintType)
struct FTime
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	uint32 Hour = 0;

	UPROPERTY(VisibleAnywhere)
	uint32 Day = 1;

	UPROPERTY(VisibleAnywhere)
	uint32 Month = 1;

	UPROPERTY(VisibleAnywhere)
	uint32 Year = 1400;

	UPROPERTY(VisibleAnywhere)
	EDayPart DayPart{ EDayPart::Night };
};

USTRUCT(BlueprintType)
struct FAMV_QuestInfo
{
	GENERATED_BODY()
	FAMV_QuestInfo() = default;
	FAMV_QuestInfo(FVector NewPosition, const UQuest* NewQuest) : Position(NewPosition), Quest(NewQuest) {}
	bool operator ==(const FAMV_QuestInfo& Rhs)
	{
		if (Position == Rhs.Position && Quest == Rhs.Quest)
		{
			return true;
		}
		return false;
	}

	UPROPERTY()
	FVector Position{ 0,0,0 };

	UPROPERTY()
	const UQuest* Quest{ nullptr };
};

USTRUCT(BlueprintType)
struct FAMV_EntityBaseInfo
{
	GENERATED_BODY()
	FAMV_EntityBaseInfo() = default;
	FAMV_EntityBaseInfo(FVector NewPosition, UMissionDA* NewMissionDA) : Position(NewPosition), MissionDA(NewMissionDA) {}
	bool operator ==(const FAMV_EntityBaseInfo& Rhs)
	{
		if (Position == Rhs.Position && MissionDA == Rhs.MissionDA)
		{
			return true;
		}
		return false;
	}

	UPROPERTY()
	FVector Position{ 0,0,0 };

	UPROPERTY()
	UMissionDA* MissionDA{ nullptr };
};

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
	const AMV_Map* Map;

	UPROPERTY()
	TArray<FAMV_EntityBaseInfo> ActiveEntitiesInfo;

	UPROPERTY()
	TArray<FAMV_QuestInfo> ActiveQuestInfo;

	UPROPERTY()
	FTime ActiveTime{};

	void SaveActiveEntities(const TArray<const AMV_EntityBase*>& ActiveEntities);
	void SaveActiveQuests(const TArray<const AMV_QuestGiver*>& ActiveQuestGivers);
	void SaveTime(const FTime& Time);
	void SaveGame();
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	const TArray<FAMV_EntityBaseInfo> RetrieveActiveEntities() const;
	const TArray<FAMV_QuestInfo> RetrieveActiveQuests() const;
	FTime RetrieveTime() const;
	void SetStartedEntity(AMV_EntityBase* EntityToStart, UMissionDA* Mission);
	void SetStartedQuest(const UQuest* QuestToStart);
	const FAMV_EntityBaseInfo GetStartedEntity() const;
	void ResolveRewards(const UMissionDA* MissionDA, const UQuest* Quest = nullptr);

	void MissionEnd(const TArray<const ASevenCharacter*>& SevenCharacters, const bool bWin);
	void UpdateSevenCharactersState(const TArray<const ASevenCharacter*>& SevenCharacters);
	UFUNCTION(BlueprintCallable)
	void AddToSelectedCharacter(USevenCharacterDA* SevenCharacterDA);
	UFUNCTION(BlueprintCallable)
	const TArray<USevenCharacterDA*> GetSelectedCharacters() const;
	void OpenLevel(const FName& LevelName);

private:
	FAMV_EntityBaseInfo GetStartedEntity();
	const FAMV_QuestInfo GetStartedQuestInfo();
};
