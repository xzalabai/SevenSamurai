#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MV_OverlapInterface.generated.h"

UINTERFACE(MinimalAPI)
class UMV_OverlapInterface : public UInterface
{
	GENERATED_BODY()
};

class SEVEN_API IMV_OverlapInterface
{
	GENERATED_BODY()

public:
	virtual void OnOverlapAction() = 0;
};
