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
CRITICAL - TURN OFF OPTIMIZATION
High - Create View For 3rd person (so it's not controlled directly in Seven
High - Make a BASE for Character, then derive for SevenCharacter and AEnemy
High - Consideration: Change OnAnimationEnded to EVENT ?
High - FIX // v ^ || > ^  for EVADINGAWAY function ! now it calculates inccorectly. Help urself with arrow Debugs
High - Decide, whether all enemies will be always facing Character. If YES -> Do nothing. If NO -> warp animation also when enemy is in back
High - Change int in AttackTypeMontage for enum -> where we define all sections!\
Medium - Remove Enemy class (and leave only Enemy Character class) and BirdView
Medium - put ComboComponent into AttackComponent
Medium - Merge all methods (look,space,move) into one with, where BindAction will have 1 parameter (delegate) ... ALSO make it more readable ..
Medium - IsBlocking -> update this variable NOT IN ANIMATION BP UPDATE but call event which sets it!
Medium - Cache player who owns a camera
Medium - Modify Weird Light Attack animation
Medium - Sync ESpecial with int that is passed into the ComboManager
Medium - OnAnimationEnded is triggered also for Victim -> although sometimes it's intended only for Attacker! replace with anim notifier probably
Medium - GetDirection -> complete for evading to 8 DIRECTIONS ! Then Change IsEvadingAway to compare with 8 evades not 4

Features:
Combat
- More Hits (reaction based on rotation)
- Effects
- Play correct Hit (from behind, side hit)
AI
Blocking
- turn in place while defending
Evading
HP
Throw
Inventory
Quest

Learn:
Understand properly what's going on in WarpAttacker!!! (Rotation of victimDesiredPoint)
Understand why CDO is ran multiple times and we can't set value as default initialized value / or in constructor: Because of https://stackoverflow.com/questions/67414701/initializing-static-variables-in-ue4-c 
Why not marking function (in a namespace in PublicEnums returns linker error)? https://stackoverflow.com/questions/15235526/the-static-keyword-and-its-various-uses-in-c
*/
