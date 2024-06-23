// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MV_AIController.generated.h"

UCLASS()
class SEVEN_API AMV_AIController : public AAIController
{
	GENERATED_BODY()

public:

private:
	bool bMovementFinished{ false };

public:
	void MoveCharacterTo(const FVector& Position);

private:
	void RequestFinished(FAIRequestID AIRequestID, const FPathFollowingResult& PathFollowingResult);

protected:
	virtual void BeginPlay() override;
};
