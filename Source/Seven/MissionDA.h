#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Mission.h"
#include "Engine/DataTable.h"
#include "MissionDA.generated.h"

class UPaperSprite;
class USevenCharacterDA;

UENUM(BlueprintType)
enum class EStatus : uint8
{
	Initialized,
	Started,
	Completed,
	Aborted,
	FinishedWithWin,
	FinishedWithLose,
};

USTRUCT(BlueprintType)
struct SEVEN_API FUMissionDT : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int UniqueID;

	UPROPERTY(EditAnywhere)
	FName Name;

	UPROPERTY(EditAnywhere)
	FName Description;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UPaperSprite> Image;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UPaperSprite> MissionCompleteImage;

	UPROPERTY(EditAnywhere)
	EMissionType MissionType;

	UPROPERTY(EditAnywhere)
	TMap<TSubclassOf<AEnemyCharacter>, int> EnemiesToSpawn;

	UPROPERTY(EditAnywhere)
	TMap<EItemType, int32> Reward;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USevenCharacterDA> SpecialCharacter;

	UPROPERTY()
	uint8 AreaIndex;

	UPROPERTY(EditAnywhere)
	uint8 Difficulty;

	UPROPERTY(EditAnywhere)
	EStatus MissionStatus{ EStatus::Initialized };
};

UCLASS(BlueprintType)
class SEVEN_API UMissionDA : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	int UniqueID;

	UPROPERTY(EditAnywhere)
	FName Name;

	UPROPERTY(EditAnywhere)
	FName Description;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UPaperSprite> Image;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UPaperSprite> MissionCompleteImage;

	UPROPERTY(EditAnywhere)
	EMissionType MissionType;

	UPROPERTY(EditAnywhere)
	TMap<TSubclassOf<AEnemyCharacter>, int> EnemiesToSpawn;

	UPROPERTY(EditAnywhere)
	TMap<EItemType, int32> Reward;

	UPROPERTY(EditAnywhere)
	USevenCharacterDA* SpecialCharacter;

	UPROPERTY()
	uint8 AreaIndex;

	UPROPERTY(EditAnywhere)
	uint8 Difficulty;

	UPROPERTY(EditAnywhere)
	EStatus MissionStatus{ EStatus::Initialized };
};
