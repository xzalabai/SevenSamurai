#pragma once
#include "CoreMinimal.h"
#include "PaperSpriteActor.h"
#include "PublicEnums.h"
#include "GameController.h"
#include "MissionDA.h"
#include "MV_Map.generated.h"

class UMissionDA;
class UPaperSpriteComponent;
class AMV_EntityBase;
class AMV_Enemy;
class AMV_QuestGiver;
class AMV_Village;
class AMVSevenCharacter;
class UBoxComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnDayPeriodChange, EDayPart /* DayPart */)

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

UCLASS()

class SEVEN_API AMV_Map : public APaperSpriteActor
{
	friend class UGameController;
	friend class AMVGameMode;

	GENERATED_BODY()

public:
	FOnDayPeriodChange OnDayPeriodChange;

private:	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	TArray<UMissionDA*> AvailableVillages;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	TArray<UMissionDA*> AvailableMissions;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	TArray<UMissionDA*> AvailableEnemies;

	UPROPERTY()
	TArray<const AMV_QuestGiver*> ActiveQuestGivers{};

	UPROPERTY()
	TArray<UMissionDA*> GeneratedMissions; // TOdo is it needed? We are saving it only to have reference count..

	UPROPERTY();
	TArray<const AMV_EntityBase*> ActiveEntities;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMV_EntityBase> MissionClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMV_EntityBase> VillageClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMV_Enemy> EnemyClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMV_QuestGiver> QuestGiverClass;

	UPROPERTY()
	TObjectPtr<AMVSevenCharacter> MVSevenCharacter;

	UPROPERTY(EditAnywhere)
	TArray<UBoxComponent*> Areas;

	UPROPERTY()
	FTime Time{};


protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	FVector GetRandomPointOnMap(const UBoxComponent* const Area = nullptr, const bool bShift = false, const int32 OverlapRadius = -1) const;
	AMV_Map();
	TObjectPtr<AMVSevenCharacter> GetMVSevenCharacter() const;

private:
	const AMV_EntityBase* GenerateEntity(const int8 Index = -1, EMissionType MissionType = EMissionType::NotProvided);
	void GenerateQuestGiver(const int8 Index = -1);
	void GenerateEntites();
	void SpawnQuestGiver(const FAMV_QuestInfo& QuestGiverToSpawn = FAMV_QuestInfo());
	const AMV_EntityBase* SpawnEntity(const FAMV_EntityBaseInfo& EntityToSpawn = FAMV_EntityBaseInfo());
	void LoadSavedEntities(const TArray<FAMV_EntityBaseInfo>& EntitiesToSpawn);
	void LoadSavedQuests(const TArray<FAMV_QuestInfo>& QuestGiversToSpawn);
	bool IsOverlappingAnyEntity(const FVector& Vector1, const int32 OverlapRadius) const;
	int32 GetActiveEnemies() const;
	
};
