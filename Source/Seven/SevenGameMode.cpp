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
High - Check answer from Kevin Samurai asset -> if he provides Non-RootMotion Guard ! ->
High - Make a BASE for Character, then derive for SevenCharacter and AEnemy
High - Refactor logic for Attacks (avoid spamming, etc) -> do it with a help of AttackEnd and AttackStart
High - Change int in AttackTypeMontage for enum -> where we define all sections!
Medium - Remove Enemy class (and leave only Enemy Character class)
Medium - Merge all methods (look,space,move) into one with, where BindAction will have 1 parameter (delegate) ... ALSO make it more readable ..
Medium - IsBlocking -> update this variable NOT IN ANIMATION BP UPDATE but call event which sets it!
Medium - Cache player who owns a camera
Medium - Sync ESpecial with int that is passed into the ComboManager
Medium - OnAnimationEnded is triggered also for Victim -> although sometimes it's intended only for Attacker! replace with anim notifier probably


Features:
AI
Blocking
Evading
HP
Throw
Inventory
Quest

Learn:
Understand why CDO is ran multiple times and we can't set value as default initialized value / or in constructor: Because of https://stackoverflow.com/questions/67414701/initializing-static-variables-in-ue4-c 

*/
