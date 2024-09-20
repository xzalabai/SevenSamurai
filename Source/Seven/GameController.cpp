#include "GameController.h"
#include "SevenCharacter.h"
#include "SevenPlayerController.h"
#include "MV_QuestGiver.h"
#include "MissionDA.h"
#include "SevenCharacterDA.h"
#include "MVSevenCharacter.h"
#include "MV_EntityBase.h"
#include "MV_Area.h"
#include "MV_Map.h"
#include "Quest.h"
#include "AttributesComponent.h"
#include "Kismet\GameplayStatics.h"
#include "Mission.h"

void UGameController::SaveActiveEntities(const TArray<const AMV_EntityBase*>& ActiveEntities)
{
	ActiveEntitiesInfo.Empty();
	for (const AMV_EntityBase* EntityBase : ActiveEntities)
	{
		ActiveEntitiesInfo.Add(FAMV_EntityBaseInfo(EntityBase->GetActorLocation(), EntityBase->GetMissionDA()));
	}
}

void UGameController::SaveActiveQuests(const TArray<const AMV_QuestGiver*>& ActiveQuestGivers)
{
	ActiveQuestInfo.Empty();
	for (const AMV_QuestGiver* QuestGiver : ActiveQuestGivers)
	{
		ActiveQuestInfo.Add(FAMV_QuestInfo(QuestGiver->GetActorLocation(), QuestGiver->GetQuest()));
	}
}

void UGameController::SaveTime(const FTime& Time)
{
	ActiveTime = Time;
}

void UGameController::SaveAreasInfo(const TArray<AMV_Area*>& Areas)
{
	AreasInfo.Empty();
	for (const AMV_Area* Area : Areas)
	{
		AreasInfo.Add(FAMV_Area{ Area->UniqueAreaID, Area->ActiveEnemiesInArea, Area->TotalEnemiesInArea, Area->AreaStatus });
	}
}

void UGameController::SaveMVSevenCharacter(const TObjectPtr<AMVSevenCharacter> MVSevenCharacter)
{
	PlayerStats.bIsCamp = MVSevenCharacter->IsCamp();
	PlayerStats.Stamina = MVSevenCharacter->GetStamina();
	PlayerStats.Position = MVSevenCharacter->GetActorLocation();
	PlayerStats.Position.X -= 200;
}

void UGameController::ProcessRewards(const bool bWin, const UMissionDA* const MissionDA)
{
	// TODO: remove this: except adding a new character
	if (bWin)
	{
		if (MissionDA->SpecialCharacter)
		{
			SelectedCharacters.Add(MissionDA->SpecialCharacter);
		}

		for (const TPair<EItemType, uint16>& Pair : MissionDA->Reward)
		{
			if (!PlayerStats.Reward.Contains(Pair.Key))
			{
				PlayerStats.Reward.Add(Pair.Key, Pair.Value);
			}
			else
			{
				PlayerStats.Reward[Pair.Key] = PlayerStats.Reward[Pair.Key] + Pair.Value;
			}
		}
	}
	else
	{
		for (const TPair<EItemType, uint16>& Pair : MissionDA->Reward)
		{
			if (!PlayerStats.Reward.Contains(Pair.Key))
			{
				PlayerStats.Reward.Add(Pair.Key, (-1)*Pair.Value);
			}
			else
			{
				PlayerStats.Reward[Pair.Key] = PlayerStats.Reward[Pair.Key] - Pair.Value;
			}
		}
	}

}

void UGameController::SaveGame()
{
	Map = Cast<AMV_Map>(UGameplayStatics::GetActorOfClass(this, AMV_Map::StaticClass()));
	SaveActiveEntities(Map->ActiveEntities);
	SaveActiveQuests(Map->ActiveQuestGivers);
	SaveTime(Map->Time);
	SaveMVSevenCharacter(Map->MVSevenCharacter);
	SaveAreasInfo(Map->Areas);
}

const TArray<FAMV_EntityBaseInfo> UGameController::RetrieveActiveEntities() const
{
	return ActiveEntitiesInfo;
}

const TArray<FAMV_QuestInfo> UGameController::RetrieveActiveQuests() const
{
	return ActiveQuestInfo;
}

const TArray<FAMV_Area> UGameController::RetrieveAreasInfo() const
{
	return AreasInfo;
}

FTime UGameController::RetrieveTime() const
{
	return ActiveTime;
}

const FAMV_EntityBaseInfo UGameController::GetStartedEntity() const
{
	for (const FAMV_EntityBaseInfo& Entity : ActiveEntitiesInfo)
	{
		if (Entity.MissionDA->MissionStatus == EStatus::Started)
		{
			return Entity;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("[UGameController].GetStartedEntity HAVEN'T FOUND STARTED MISSION!!!"));
	return FAMV_EntityBaseInfo();
}

FAMV_EntityBaseInfo UGameController::GetStartedEntity()
{
	// Returning as Value because can be null
	for (FAMV_EntityBaseInfo& Entity : ActiveEntitiesInfo)
	{
		if (Entity.MissionDA->MissionStatus == EStatus::Started)
		{
			return Entity;
		}
	}
	UE_LOG(LogTemp, Error, TEXT("[UGameController].GetStartedEntity HAVEN'T FOUND STARTED MISSION!!!"));
	return FAMV_EntityBaseInfo();
}

const FAMV_QuestInfo UGameController::GetStartedQuestInfo()
{
	for (const FAMV_QuestInfo& QuestInfo : ActiveQuestInfo)
	{
		if (QuestInfo.Quest->QuestStatus == EStatus::Started)
		{
			return QuestInfo;
		}
	}
	return FAMV_QuestInfo();
}

void UGameController::Initialize(FSubsystemCollectionBase& Collection)
{
	UniqueIDCounter = 0;
	SelectedCharacters.Empty();
}

void UGameController::SetStartedEntity(AMV_EntityBase* EntityToStart, UMissionDA* Mission)
{
	SaveGame();
	Mission->MissionStatus = EStatus::Started;
	// TODO CHECK IF THE STARTED MISSION IS MARKED AS TRUE!!!! since
	OpenLevel(FName("ThirdPersonMap"));
}

void UGameController::SetStartedQuest(const UQuest* QuestToStart)
{
	SaveGame();
	QuestToStart->QuestStatus = EStatus::Started;
	
}

void UGameController::EnterVillage(const int VillageID)
{
	VisitedVillageID = VillageID;
	UE_LOG(LogTemp, Warning, TEXT("[UGameController].EnterVillage %d"), VillageID);
	SaveGame();
	OpenLevel(FName("VillageView"));
}

void UGameController::ExitVillage(const int VillageID)
{
	VisitedVillageID = -1;
	OpenLevel(FName("Map"));
}

void UGameController::MissionEnd(const TArray<const ASevenCharacter*>& SevenCharacters, const bool bWin)
{
	UpdateSevenCharactersState(SevenCharacters);
	const FAMV_QuestInfo QuestInfo = GetStartedQuestInfo();
	const FAMV_EntityBaseInfo Entity = GetStartedEntity();

	if (bWin)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UGameController].MissionEnd Character WON! Changing the Entity to be open!"));
		Entity.MissionDA->MissionStatus = EStatus::Completed; // this looks like we modify value, but we modify pointer and address stored.
		
		if (QuestInfo.Quest)
		{
			// Finished Mission was part of an active quest.
			QuestInfo.Quest->QuestStatus = EStatus::Completed;
			ActiveQuestInfo.RemoveSwap(QuestInfo);
		}
		
		if (Entity.MissionDA->MissionType == EMissionType::Enemy)
		{
			// TODO: This should be elsewhere .. handled in MV_Map?
			AreasInfo[Entity.MissionDA->AreaIndex].ActiveEnemiesInArea -= 1;
		}

		// Remove Mission from ActiveEntities
		//ActiveEntitiesInfo.RemoveSwap(Entity);
	}
	else
	{
		if (QuestInfo.Quest)
		{
			QuestInfo.Quest->QuestStatus = EStatus::Aborted;
		}
		// flee away with a character
		UE_LOG(LogTemp, Warning, TEXT("[UGameController].MissionEnd Character LOST!"));
	}
	ProcessRewards(bWin, Entity.MissionDA);
	OpenLevel(FName("Map"));	
}

void UGameController::UpdateSevenCharactersState(const TArray<const ASevenCharacter*>& SevenCharacters)
{
	for (const ASevenCharacter* SevenCharacter : SevenCharacters)
	{
		if (SevenCharacter->IsAlive())
		{
			const int32 Index = SelectedCharacters.Find(SevenCharacter->SevenCharacterDA);
			if (Index == INDEX_NONE)
			{
				UE_LOG(LogTemp, Error, TEXT("[UGameController].UpdateSevenCharactersState Unable to find SevenCharacterDA amongst player's SelectedCharacters!\n Might be just debug player"));
			}
			else
			{
				SelectedCharacters[Index]->HP = SevenCharacter->AC_Attribute->GetHP();
			}
		}
		else
		{
			// Remove from playable characters
			SelectedCharacters.RemoveSwap(SevenCharacter->SevenCharacterDA);
		}
	}
}

void UGameController::UpdateSevenCharactersHP(const uint16 Amount)
{
	for (USevenCharacterDA* SevenCharacterDA : SelectedCharacters)
	{
		//uint8 NewHP = Amount + SevenCharacterDA->HP;
		//SevenCharacterDA->HP = FMath::Min(NewHP, SevenCharacterDA->MaxHP);
	}
}

void UGameController::AddToSelectedCharacter(USevenCharacterDA* SevenCharacterDA)
{
	UE_LOG(LogTemp, Display, TEXT("[UGameController].AddToSelectedCharacter New character added!"));
	SelectedCharacters.Add(SevenCharacterDA);
	//SevenCharacterDA->bWasUsed = true;
	for (USevenCharacterDA* SelectedCharacter : SelectedCharacters)
	{
		UE_LOG(LogTemp, Display, TEXT("[UGameController].AddToSelectedCharacter Character: %s is in selectedCharacter"), *SelectedCharacter->Name.ToString());
	}
	
	const int32_t Idx = SevenCharactersData.Add(NewObject<USevenCharacterData>());
	SevenCharactersData[Idx]->SevenCharacterDA = SevenCharacterDA;
}

const TArray<USevenCharacterDA*> UGameController::GetSelectedCharacters() const
{
	return SelectedCharacters;
}

void UGameController::OpenLevel(const FName& LevelName)
{
	UE_LOG(LogTemp, Warning, TEXT("[UGameController].OpenLevel %s"), *LevelName.ToString());
	UGameplayStatics::OpenLevel(this, LevelName);
}
