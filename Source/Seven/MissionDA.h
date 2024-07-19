#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Mission.h"
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

UCLASS()
class SEVEN_API UMissionDA : public UDataAsset
{
	GENERATED_BODY()

public:
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
	TMap<int, TSubclassOf<AEnemyCharacter>> EnemiesToSpawn;

	UPROPERTY(EditAnywhere)
	TMap<EItemType, int32> Reward;

	UPROPERTY(EditAnywhere)
	USevenCharacterDA* SpecialCharacter;

	UPROPERTY()
	uint8 AreaIndex;

	UPROPERTY(EditAnywhere)
	EStatus MissionStatus{ EStatus::Initialized };

};
