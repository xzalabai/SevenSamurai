#include "GameController.h"
#include "SevenCharacter.h"
#include "SevenPlayerController.h"
#include "MissionDA.h"
#include "MV_EntityBase.h"
#include "MV_Map.h"
#include "AttributesComponent.h"
#include "Kismet\GameplayStatics.h"
#include "Mission.h"

void UGameController::StoreActiveEntities(const TArray<const AMV_EntityBase*> ActiveEntities)
{
	for (const AMV_EntityBase* EntityBase : ActiveEntities)
	{
		UE_LOG(LogTemp, Warning, TEXT("MyCharacter's Location is %s"),
			*EntityBase->GetActorLocation().ToString());
		ActiveEntitiesInfo.Add(FAMV_EntityBaseInfo(EntityBase->GetActorLocation(), EntityBase->GetMissionDA()));
	}
}

const TArray<FAMV_EntityBaseInfo> UGameController::RetrieveActiveEntities() const
{
	return ActiveEntitiesInfo;
}

void UGameController::Initialize(FSubsystemCollectionBase& Collection)
{
	UniqueIDCounter = 0;
	SelectedCharacters.Empty();
}

void UGameController::SetActiveMission(const UMissionDA* Mission)
{
	Map = Cast<AMV_Map>(UGameplayStatics::GetActorOfClass(this, AMV_Map::StaticClass()));
	StoreActiveEntities(Map->ActiveEntities);

	ActiveMission = Mission;
	UGameplayStatics::OpenLevel(this, FName("ThirdPersonMap"));
}

void UGameController::MissionEnd(const TArray<const ASevenCharacter*>& SevenCharacters, const bool bWin)
{
	UpdateSevenCharactersState(SevenCharacters);
	UGameplayStatics::OpenLevel(this, FName("Map"));
	// TODO: NOW AFTER MAP IS READY, PLACE ENTITIES
	
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
