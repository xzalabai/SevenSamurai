#include "GameController.h"
#include "SevenCharacter.h"
#include "SevenPlayerController.h"
#include "MV_QuestGiver.h"
#include "MissionDA.h"
#include "SevenCharacterDA.h"
#include "MVSevenCharacter.h"
#include "MV_EntityBase.h"
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

void UGameController::SaveMVSevenCharacter(const TObjectPtr<AMVSevenCharacter> MVSevenCharacter)
{
	PlayerStats.bIsCamp = MVSevenCharacter->IsCamp();
	PlayerStats.Stamina = MVSevenCharacter->GetStamina();
	PlayerStats.Position = MVSevenCharacter->GetActorLocation();
	PlayerStats.Position.X -= 200;
}

void UGameController::ProcessRewards(const UMissionDA* const MissionDA)
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

void UGameController::SaveGame()
{
	Map = Cast<AMV_Map>(UGameplayStatics::GetActorOfClass(this, AMV_Map::StaticClass()));
	SaveActiveEntities(Map->ActiveEntities);
	SaveActiveQuests(Map->ActiveQuestGivers);
	SaveTime(Map->Time);
	SaveMVSevenCharacter(Map->MVSevenCharacter);
}

const TArray<FAMV_EntityBaseInfo> UGameController::RetrieveActiveEntities() const
{
	return ActiveEntitiesInfo;
}

const TArray<FAMV_QuestInfo> UGameController::RetrieveActiveQuests() const
{
	return ActiveQuestInfo;
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
		// Change to completed
		UE_LOG(LogTemp, Warning, TEXT("[UGameController].MissionEnd Character WON! Changing the Entity to be open!"));
		Entity.MissionDA->MissionStatus = EStatus::Completed;
		
		ProcessRewards(Entity.MissionDA);
		
		if (QuestInfo.Quest)
		{
			// Finished Mission was part of an active quest.
			QuestInfo.Quest->QuestStatus = EStatus::Completed;
			ActiveQuestInfo.RemoveSwap(QuestInfo);
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
	OpenLevel(FName("Map"));	
}

void UGameController::UpdateSevenCharactersState(const TArray<const ASevenCharacter*>& SevenCharacters)
{
	for (const ASevenCharacter* SevenCharacter : SevenCharacters)
	{
		if (SevenCharacter->IsAlive())
		{
			int32 Index = SelectedCharacters.Find(SevenCharacter->SevenCharacterDA);
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
		uint16 NewHP = Amount + SevenCharacterDA->HP;
		SevenCharacterDA->HP = FMath::Min(NewHP, SevenCharacterDA->MaxHP);
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
	UE_LOG(LogTemp, Warning, TEXT("[UGameController].OpenLevel %s"), *LevelName.ToString());
	UGameplayStatics::OpenLevel(this, LevelName);
}
