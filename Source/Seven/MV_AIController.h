// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MV_AIController.generated.h"

class AMV_Map;

UCLASS()
class SEVEN_API AMV_AIController : public AAIController
{
	GENERATED_BODY()

public:

private:
	bool bMovementFinished{ false };
	
	UPROPERTY()
	TObjectPtr<AMV_Map> Map;


public:
	void MoveCharacterTo(const FVector& Position);
	UFUNCTION(BlueprintCallable)
	void MoveToRandomPosition();

	UFUNCTION(BlueprintCallable)
	void MoveToSevenCharacter();

private:
	void RequestFinished(FAIRequestID AIRequestID, const FPathFollowingResult& PathFollowingResult);

protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsMovementFinished();
};
