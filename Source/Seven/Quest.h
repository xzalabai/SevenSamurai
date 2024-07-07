#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Quest.generated.h"

class UMissionDA;

UCLASS()
class SEVEN_API UQuest : public UObject
{
	GENERATED_BODY()

	friend class AMV_QuestGiver;
	friend class AMV_Map;

private:
	UPROPERTY()
	FName Name;

	UPROPERTY()
	UMissionDA *Mission;
};
