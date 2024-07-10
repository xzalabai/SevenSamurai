#pragma once

#include "CoreMinimal.h"
#include "VV_EntityBase.h"
#include "MV_QuestGiver.generated.h"

class UQuest;
UCLASS()
class SEVEN_API AMV_QuestGiver : public AVV_EntityBase
{
	GENERATED_BODY()

	friend class AMV_Map;

private:
	UPROPERTY()
	mutable const UQuest* Quest;

public:
	const UQuest* GetQuest() const;

protected:
	virtual void OnOverlapAction() override;
		
};
