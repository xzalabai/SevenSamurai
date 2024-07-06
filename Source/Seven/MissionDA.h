#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Mission.h"
#include "MissionDA.generated.h"

class UPaperSprite;
class USevenCharacterDA;

UENUM(BlueprintType)
enum class EMissionStatus : uint8
{
	Started,
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
	TMap<EItemType, int> Reward;

	UPROPERTY(EditAnywhere)
	USevenCharacterDA* SpecialCharacter;

	UPROPERTY(EditAnywhere)
	bool bCompleted{ false };

	UPROPERTY(EditAnywhere)
	bool bStarted{ false };

};
