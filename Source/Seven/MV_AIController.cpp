// Fill out your copyright notice in the Description page of Project Settings.


#include "MV_AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

void AMV_AIController::MoveCharacterTo(const FVector& Position)
{
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, Position);
}
