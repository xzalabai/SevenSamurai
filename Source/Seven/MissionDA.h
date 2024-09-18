#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Mission.h"
#include "Containers/Map.h"
#include "Engine/DataTable.h"
#include "EnemyLevelDA.h"
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
struct SEVEN_API FEnemyToSpawn
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	ESevenCharacterType SevenCharacterType{ ESevenCharacterType::NotProvided }; // TODO: change so we select only ESevenCharacterType and then in Mission.cpp it will load the BP!

	UPROPERTY(EditAnywhere)
	UEnemyLevelDA* EnemyLevelDA{ nullptr };

	UPROPERTY(EditAnywhere)
	int Amount;

};

USTRUCT(BlueprintType)
struct SEVEN_API FUMissionDT : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int UniqueID;

	UPROPERTY(EditAnywhere)
	FName Name; // TODO: can we remove it? it's just a name

	UPROPERTY(EditAnywhere)
	FName Description;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UPaperSprite> Image;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UPaperSprite> MissionCompleteImage;

	UPROPERTY(EditAnywhere)
	EMissionType MissionType;

	UPROPERTY(EditAnywhere)
	TArray<FEnemyToSpawn> EnemiesToSpawn;

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
	EMissionType MissionType; //TODO unused

	UPROPERTY(EditAnywhere)
	TArray<FEnemyToSpawn> EnemiesToSpawn;

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
