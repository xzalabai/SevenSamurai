#include "GameController.h"
#include "SevenCharacter.h"
#include "SevenPlayerController.h"
#include "MissionDA.h"
#include "MV_EntityBase.h"
#include "MV_Map.h"
#include "AttributesComponent.h"
#include "Kismet\GameplayStatics.h"
#include "Mission.h"

void UGameController::StoreActiveEntities(const TArray<const AMV_EntityBase*>& ActiveEntities)
{
	ActiveEntitiesInfo.Empty();
	for (const AMV_EntityBase* EntityBase : ActiveEntities)
	{
		ActiveEntitiesInfo.Add(FAMV_EntityBaseInfo(EntityBase->GetActorLocation(), EntityBase->GetMissionDA()));
	}
}

const TArray<FAMV_EntityBaseInfo> UGameController::RetrieveActiveEntities() const
{
	return ActiveEntitiesInfo;
}

const FAMV_EntityBaseInfo& UGameController::GetStartedEntity() const
{
	for (const FAMV_EntityBaseInfo& Entity : ActiveEntitiesInfo)
	{
		if (Entity.MissionDA->bStarted)
		{
			return Entity;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("[UGameController].GetStartedEntity HAVEN'T FOUND STARTED MISSION!!!"));
	return ActiveEntitiesInfo[0];
}

FAMV_EntityBaseInfo& UGameController::GetStartedEntity()
{
	for (FAMV_EntityBaseInfo& Entity : ActiveEntitiesInfo)
	{
		if (Entity.MissionDA->bStarted)
		{
			return Entity;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("[UGameController].GetStartedEntity HAVEN'T FOUND STARTED MISSION!!!"));
	return ActiveEntitiesInfo[0];
}

void UGameController::Initialize(FSubsystemCollectionBase& Collection)
{
	UniqueIDCounter = 0;
	SelectedCharacters.Empty();
}

void UGameController::SetStartedEntity(AMV_EntityBase* EntityToStart, const UMissionDA* Mission)
{
	Map = Cast<AMV_Map>(UGameplayStatics::GetActorOfClass(this, AMV_Map::StaticClass()));
	// TODO: REFACTOR, make it more efficient pls ... 
	// do not REODER now.
	StoreActiveEntities(Map->ActiveEntities);
	for (FAMV_EntityBaseInfo& Entity : ActiveEntitiesInfo)
	{
		if (Entity.MissionDA == Mission)
		{
			Entity.MissionDA->bStarted = true;
		}
	}
	OpenLevel(FName("ThirdPersonMap"));
}

void UGameController::MissionEnd(const TArray<const ASevenCharacter*>& SevenCharacters, const bool bWin)
{
	UpdateSevenCharactersState(SevenCharacters);
	
	if (bWin)
	{
		// Change to completed
		UE_LOG(LogTemp, Warning, TEXT("[UGameController].MissionEnd Character WON! Changing the Entity to be open!"));
		FAMV_EntityBaseInfo& Entity = GetStartedEntity();
		Entity.MissionDA->bCompleted = true;
		Entity.MissionDA->bStarted = false;
		// TODO: Process reward!


		if (Entity.MissionDA->SpecialCharacter)
		{
			SelectedCharacters.Add(Entity.MissionDA->SpecialCharacter);
		}
	}
	else
	{
		// flee away with a character
		UE_LOG(LogTemp, Warning, TEXT("[UGameController].MissionEnd Character LOST!"));
	}

	OpenLevel(FName("Map"));	
}

void UGameController::UpdateSevenCharactersState(const TArray<const ASevenCharacter*>& SevenCharacters)
{
	for (const ASevenCharacter* SevenCharacter : SevenCharacters)
	{
		if (!SevenCharacter->IsAlive())
		{
			SelectedCharacters.RemoveSwap(SevenCharacter->SevenCharacterDA);
		}
	}
}

void UGameController::AddToSelectedCharacter(USevenCharacterDA* SevenCharacterDA)
{
	SelectedCharacters.Add(SevenCharacterDA);
}

const TArray<USevenCharacterDA*> UGameController::GetSelectedCharacters() const
{
	return SelectedCharacters;
}

void UGameController::OpenLevel(const FName& LevelName)
{
	UGameplayStatics::OpenLevel(this, LevelName);
}
