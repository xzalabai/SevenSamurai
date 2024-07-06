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
class AMission;
class AMV_EntityBase;

static uint8 UniqueIDCounter;

USTRUCT(BlueprintType)
struct FAMV_EntityBaseInfo
{
	GENERATED_BODY()
	FAMV_EntityBaseInfo() = default;
	FAMV_EntityBaseInfo(FVector NewPosition, UMissionDA* NewMissionDA) : Position(NewPosition), MissionDA(NewMissionDA) {}

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

	void StoreActiveEntities(const TArray<const AMV_EntityBase*> ActiveEntities);
public:
	const TArray<FAMV_EntityBaseInfo> RetrieveActiveEntities() const;
	const FAMV_EntityBaseInfo& GetStartedEntity() const;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	void SetStartedEntity(AMV_EntityBase* EntityToStart, const UMissionDA* Mission);
	void MissionEnd(const TArray<const ASevenCharacter*>& SevenCharacters, const bool bWin);
	void UpdateSevenCharactersState(const TArray<const ASevenCharacter*>& SevenCharacters);
	UFUNCTION(BlueprintCallable)
	void AddToSelectedCharacter(USevenCharacterDA* SevenCharacterDA);
	UFUNCTION(BlueprintCallable)
	const TArray<USevenCharacterDA*> GetSelectedCharacters() const;
	void OpenLevel(const FName& LevelName);

private:
	FAMV_EntityBaseInfo& GetStartedEntity();
};
