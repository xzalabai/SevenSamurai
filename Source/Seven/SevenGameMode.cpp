// Copyright Epic Games, Inc. All Rights Reserved.

#include "SevenGameMode.h"
#include "SevenCharacter.h"
#include "SevenPlayerController.h"
#include "LootGenerator.h"
#include "Kismet\GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"


ASevenGameMode::ASevenGameMode()
{
	AC_LootGenerator = CreateDefaultSubobject<ULootGenerator>(TEXT("LootGenerator"));
}
/*
TODOs:

Refactor:
CRITICAL - TURN OFF OPTIMIZATION
CRITICAL - As a part of more characters, delete SevenCharacterToAttack and find out logic for marking WHO is character going to attack.
Maybe - Create View For 3rd person (so it's not controlled directly in Seven
High - make sync between AIController, EnemyCharacter, TargetedEnemy, EnemyToAttack etc... so they it is being choosed in AIController and everyone follows it
High - Fix bug when Heavy Attack is loading and player receives hit.
High - Make a BASE for Character, then derive for SevenCharacter and AEnemy
High - rename Combo to LightCombo or smth.
High - Consideration: Change OnAnimationEnded to EVENT ?
High - FIX // v ^ || > ^  for EVADINGAWAY function ! now it calculates inccorectly. Help urself with arrow Debugs
High - Decide, whether all enemies will be always facing Character. If YES -> Do nothing. If NO -> warp animation also when enemy is in back
High - Change int in AttackTypeMontage for enum -> where we define all sections!\
Medium - Move HP to Attributes -> and find out why it's crashing
Medium - adjust Hit Reaction animations based on the direction!
Medium - Change logic of OnLayingDead .. it should be controlled from Controller / Animation / Character ?
Medium - Merge all methods (look,space,move) into one with, where BindAction will have 1 parameter (delegate) ... ALSO make it more readable ..
Medium - IsBlocking -> update this variable NOT IN ANIMATION BP UPDATE but call event which sets it!
Medium - Cache player who owns a camera
Medium - GetDirection -> complete for evading to 8 DIRECTIONS ! Then Change IsEvadingAway to compare with 8 evades not 4
Medium - Adjust Fire as is in FireRMB
Medium - Put relevant functions from FInputValue to ETriggerEvent (as is in FireRMB)
Medium - remove GET_ prefix from getters and use IS_


Features:
Combat
- Effects
- Play correct Hit (from behind, side hit)
- Drop also Seven Character's items if he dies
AI
Blocking
- turn in place while defending
- Forbit blocking to AI when hitting it!
Evading (small evade)
Inventory
Quest

Learn:
Understand why I can't make move assigment if there is no other copy assignemnt / copy constructor / move constructor present
Understand properly what's going on in WarpAttacker!!! (Rotation of victimDesiredPoint)
Understand why CDO is ran multiple times and we can't set value as default initialized value / or in constructor: Because of https://stackoverflow.com/questions/67414701/initializing-static-variables-in-ue4-c 
Why not marking function (in a namespace in PublicEnums returns linker error)? https://stackoverflow.com/questions/15235526/the-static-keyword-and-its-various-uses-in-c
*/
