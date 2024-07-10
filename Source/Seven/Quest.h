#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MissionDA.h"
#include "Quest.generated.h"


UCLASS()
class SEVEN_API UQuest : public UObject
{
	GENERATED_BODY()

	friend class AMV_QuestGiver;
	friend class AMV_Map;
	friend class UGameController;

private:
	UPROPERTY()
	FName Name;

	UPROPERTY()
	mutable EStatus QuestStatus{ EStatus::Initialized };

	UPROPERTY()
	UMissionDA *Mission;
};
