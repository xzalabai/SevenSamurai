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
Maybe - Create View For 3rd person (so it's not controlled directly in Seven
High - Put OnStatusUpdate to ASevenGameMode
High - make sync between AIController, EnemyCharacter, TargetedEnemy, EnemyToAttack etc... so they it is being choosed in AIController and everyone follows it
High - FIND OUT WHY AC_ATTACKCOMPONENT cannot have UPROPERTY() - bc it's null
High - Fix weird Character rotation after some animation is performed on steep surface
High - Change MissionHandler and ComboManager to SubSystem
High - Fix bug when Heavy Attack is loading and player receives hit.
High - Make a BASE for Character, then derive for SevenCharacter and AEnemy
High - rename Combo to LightCombo or smth.
High - Consideration: Change OnAnimationEnded to EVENT ?
High - Refactor logic of returning AttackToken (now in AICharacter there is assumption in Fire() that it is enemy
High - FIX // v ^ || > ^  for EVADINGAWAY function ! now it calculates inccorectly. Help urself with arrow Debugs
High - ADD Listener in CharacterPicker also to the SIDE MISSION!!!
High - Fix Guard left walk (character seems to go forward)
Medium - Find better solution to store MissionType for EnemyCharacter and then resolve it based on DataAsset stored in EnemyController (expensive!)
Medium - Move HP to Attributes -> and find out why it's crashing
Medium - Handle behavior if SevenCharacter got deleted (with all combos, weapon upgrades -> might be weird for SevenCharacterDA, CharacterPicker...)
Medium - adjust Hit Reaction animations based on the direction!
Medium - Change logic of OnLayingDead .. it should be controlled from Controller / Animation / Character ?
Medium - Merge all methods (look,space,move) into one with, where BindAction will have 1 parameter (delegate) ... ALSO make it more readable ..
Medium - IsBlocking -> update this variable NOT IN ANIMATION BP UPDATE but call event which sets it!
Medium - Cache player who owns a camera
Medium - GetDirection -> complete for evading to 8 DIRECTIONS ! Then Change IsEvadingAway to compare with 8 evades not 4
Medium - Adjust Fire as is in FireRMB
Medium - Put relevant functions from FInputValue to ETriggerEvent (as is in FireRMB)
OBSOLETE Medium - remove GET_ prefix from getters and use IS_


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
Why I cannot assign use APlayerController* plcntrl = SevenCharacter->GetPlayerController (which returns ASevenCharacterController)  ????
Understand why I can't make move assigment if there is no other copy assignemnt / copy constructor / move constructor present
Understand properly what's going on in WarpAttacker!!! (Rotation of victimDesiredPoint)
Understand why CDO is ran multiple times and we can't set value as default initialized value / or in constructor: Because of https://stackoverflow.com/questions/67414701/initializing-static-variables-in-ue4-c 
Why not marking function (in a namespace in PublicEnums returns linker error)? https://stackoverflow.com/questions/15235526/the-static-keyword-and-its-various-uses-in-c
*/
