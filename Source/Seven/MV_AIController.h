// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PublicEnums.h"
#include "MV_AIController.generated.h"

class AMV_Map;

UCLASS()
class SEVEN_API AMV_AIController : public AAIController
{
	GENERATED_BODY()

private:
	bool bMovementFinished{ false };
	bool bControllingSevenCharacter{ false };
	
	UPROPERTY()
	TObjectPtr<AMV_Map> Map;

public:
	void MoveCharacterTo(const FVector& Position);
	UFUNCTION(BlueprintCallable)
	void MoveToRandomPosition();

	UFUNCTION(BlueprintCallable)
	void MoveToSevenCharacter();

	void EnableMoving(const bool bEnable);

private:
	void RequestFinished(FAIRequestID AIRequestID, const FPathFollowingResult& PathFollowingResult);
	void OnDayPeriodChange(EDayPart DayPart);

protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsMovementFinished();
	UFUNCTION(BlueprintCallable)
	void Camp(const bool bEnable) const;

};
