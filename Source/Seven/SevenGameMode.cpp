// Copyright Epic Games, Inc. All Rights Reserved.

#include "SevenGameMode.h"
#include "SevenCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASevenGameMode::ASevenGameMode()
{

	// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
}
