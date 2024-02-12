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
Medium - Merge all methods (look,space,move) into one with, where BindAction will have 1 parameter (delegate) ... ALSO make it more readable ..
Medium - Cache player who owns a camera

Features:
High - Instead of caching 1 enemy, in AttackStart launch SphereTrace (in front of a player) and for found Enemies iterate and find closest one -> if dot is in treshold, rotate towards him
high - Remove existing combo, Create new Combo class (inherit from UObject), do it via Subclassof ? 



*/
