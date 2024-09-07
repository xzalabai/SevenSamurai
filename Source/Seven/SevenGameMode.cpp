#include "SevenGameMode.h"
#include "SevenCharacter.h"
#include "SevenCharacterDA.h"
#include "GameController.h"
#include "SevenPlayerController.h"
#include "LootGenerator.h"
#include "Kismet\GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"


ASevenGameMode::ASevenGameMode()
{
	AC_LootGenerator = CreateDefaultSubobject<ULootGenerator>(TEXT("LootGenerator"));
	CachedSevenPlayerController = nullptr;
}

void ASevenGameMode::UpdateStatus(const ASevenCharacter* SevenCharacter, const ECharacterState Status)
{
	const int8 CharacterID = SevenCharacter->GetUniqueID();

	if (SevenCharacter && SevenCharacter->IsEnemy())
	{
		// Updating list of enemies
		if (!EnemiesStatus.Contains(CharacterID))
		{
			EnemiesStatus.Add({ CharacterID, Status });
			Enemies.Add(SevenCharacter);
		}
		else
		{
			EnemiesStatus[CharacterID] = Status;
		}
	}
	else
	{
		// Updating list of SevenCharactersStatus (playable characters)
		if (!SevenCharactersStatus.Contains(CharacterID))
		{
			SevenCharactersStatus.Add({ CharacterID, Status });
			SevenCharacters.Add(SevenCharacter);
		}
		else
		{
			SevenCharactersStatus[CharacterID] = Status;
		}
		if (SevenCharacter->IsPlayerControlled())
		{
			OnSevenCharacterStatusUpdate.Broadcast(SevenCharacter, Status);
		}
	}

	OnStatusUpdate.Broadcast(SevenCharacter, Status);

	if (SevenCharacter == GetPossessedCharacter() && Status == ECharacterState::Dead)
	{
		ASevenPlayerController* SevenPlayerController = Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		SevenPlayerController->UpdateDeadCharacter(SevenCharacter->SevenCharacterDA->Name);
		SevenPlayerController->Switch(FInputActionValue());
	}
}

ASevenCharacter* ASevenGameMode::GetAnyAliveEnemy()
{
	for (const ASevenCharacter* Enemy : Enemies)
	{
		if (Enemy->IsAlive())
		{
			return const_cast<ASevenCharacter*>(Enemy);
		}
	}
	UE_LOG(LogTemp, Error, TEXT("[ASevenPlayerController].GetAnyAliveEnemy No alive enemies left."));
	return nullptr;
}

ASevenCharacter* ASevenGameMode::GetPossessedCharacter() const
{
	if (!CachedSevenPlayerController)
	{
		CachedSevenPlayerController = GetSevenPlayerController();
	}
	return CachedSevenPlayerController->GetPawn<ASevenCharacter>();
}

ASevenPlayerController* ASevenGameMode::GetSevenPlayerController() const
{
	return Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

TMap<int8, ECharacterState> ASevenGameMode::GetSevenCharactersStatus() const
{
	return SevenCharactersStatus;
}

TArray<const ASevenCharacter*> ASevenGameMode::GetAIControlledAllies() const
{
	TArray<const ASevenCharacter*> AIControlledAllies;
	const ASevenCharacter* PlayerControlledAlly = GetPossessedCharacter();

	for (const ASevenCharacter* Seven : SevenCharacters)
	{
		if (Seven == PlayerControlledAlly || !Seven->IsAlive())
		{
			continue;
		}
		AIControlledAllies.Add(Seven);
	}

	return AIControlledAllies;
}

const TArray<const ASevenCharacter*> ASevenGameMode::GetSevenCharacters() const
{
	return SevenCharacters;
}

bool ASevenGameMode::HasAnyEnemyStatus(const ECharacterState& Status) const
{
	for (auto& Enemy : EnemiesStatus)
	{
		if (Enemy.Value == Status)
		{
			return true;
		}
	}
	return false;
}

bool ASevenGameMode::HasAnyEnemyStatus(const TArray<ECharacterState>& Status) const
{
	for (const ECharacterState& State : Status)
	{
		if (HasAnyEnemyStatus(State))
		{
			return true;
		}
	}
	return false;
}

const ECharacterState ASevenGameMode::GetEnemyStatus(const int8 CharacterID) const
{
	return (EnemiesStatus.Contains(CharacterID) ? EnemiesStatus[CharacterID] : SevenCharactersStatus[CharacterID]);
}

void ASevenGameMode::MissionEnd(bool bWin)
{
	UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	if (!bIsDebugBattle)
	{
		GameController->MissionEnd(SevenCharacters, bWin);
	}
}

void ASevenGameMode::UpdateMissionParameters(AMission* Mission)
{
	UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	const FAMV_EntityBaseInfo& StartedEntity = GameController->GetStartedEntity();

	if (StartedEntity.Position != FVector(0,0,0)) 
	{
		Mission->MissionDA = StartedEntity.MissionDA;
	}
	else
	{
		// Debug battle
		UE_LOG(LogTemp, Error, TEXT("[ASevenPlayerController].UpdateMissionParameters RUNNING DEBUG BATTLE."));
		bIsDebugBattle = true;
	}
}


/*
TODOs:

Refactor:
CRITICAL - TURN OFF OPTIMIZATION
CRITICAL - Create ControllableInterface for controlling characters, so there is no CAST in PlayerController every key !!!!!!!
Maybe - Create View For 3rd person (so it's not controlled directly in Seven
High - Remove all GetActorOfClass (also in blueprints - BP_MV_PlayerController etc
High - Remove ComboManager and move newly created structs to PublicEnums (I guess)
High - make sync between AIController, EnemyCharacter, TargetedEnemy, EnemyToAttack etc... so they it is being choosed in AIController and everyone follows it
High - rename Combo to LightCombo or smth.
High - Refactor logic of returning AttackToken (now in AICharacter there is assumption in Fire() that it is enemy
High - Split MV_EntityBase to MV_Entity (logic with Missions), and MV_EntityBase (only images) - and replace references with MV_EntityBase with MV_Entity then
     - Also replace MV_QuestGiver's parent (VV_EntityBase) with MV_EntityBase!!
High - FIX // v ^ || > ^  for EVADINGAWAY function ! now it calculates inccorectly. Help urself with arrow Debugs
High - Fix Guard left walk (character seems to go forward)
Medium - Remove Dynamic_multicast_Delegate and replace with Multicast_delegate
Medium - Refactor maps and arrays in SevenGameMode, there is no need to have so many: enemies, enemiesstatus, sevencharacters, sevencharacterstatus
Medium - Find better solution to store MissionType for EnemyCharacter and then resolve it based on DataAsset stored in EnemyController (expensive!)
Medium - Move things that are getters (GetActorOfClass from Map to GameController, and then in AMV_Map::BeginPlay just retrieve it from GameController...
Medium - adjust Hit Reaction animations based on the direction!
Medium - Change logic of OnLayingDead .. it should be controlled from Controller / Animation / Character ?
Medium - IsBlocking -> update this variable NOT IN ANIMATION BP UPDATE but call event which sets it!
Medium - GetDirection -> complete for evading to 8 DIRECTIONS ! Then Change IsEvadingAway to compare with 8 evades not 4
Medium - LoadSavedQuests LoadSavedEntities ... maybe merge those methods together?
Medium - Put relevant functions from FInputValue to ETriggerEvent (as is in FireRMB)

Features:
Combat
- Effects
- Play correct Hit (from behind, side hit)
- Drop also Seven Character's items if he dies
AI
Blocking
Evading (small evade)

Learn:
How to change value of variable where pointer points to?
Why I cannot assign use APlayerController* plcntrl = SevenCharacter->GetPlayerController (which returns ASevenCharacterController)  ????
Understand why I can't make move assigment if there is no other copy assignemnt / copy constructor / move constructor present
Understand properly what's going on in WarpAttacker!!! (Rotation of victimDesiredPoint)
Understand why CDO is ran multiple times and we can't set value as default initialized value / or in constructor: Because of https://stackoverflow.com/questions/67414701/initializing-static-variables-in-ue4-c 
Why not marking function (in a namespace in PublicEnums returns linker error)? https://stackoverflow.com/questions/15235526/the-static-keyword-and-its-various-uses-in-c
*/
