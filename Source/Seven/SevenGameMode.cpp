// Copyright Epic Games, Inc. All Rights Reserved.

#include "SevenGameMode.h"
#include "SevenCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASevenGameMode::ASevenGameMode()
{

}

/*
TODOs:

Refactor:
High - Create View For 3rd person (so it's not controlled directly in Seven
High - Make a BASE for Character, then derive for SevenCharacter and AEnemy
High - Change int in AttackTypeMontage for enum -> where we define all sections!
Medium - Merge all methods (look,space,move) into one with, where BindAction will have 1 parameter (delegate) ... ALSO make it more readable ..
Medium - Cache player who owns a camera
Medium - Sync ESpecial with int that is passed into the ComboManager

Features:
Lock (Secretly) Enemy and after every animation attack it.
Evade
Block
Special Attack -> USE MOTION WARPING https://www.youtube.com/watch?v=TuiDgXCarP0&t=10s&ab_channel=GorkaGames 



*/
