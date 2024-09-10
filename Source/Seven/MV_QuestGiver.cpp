#include "MV_QuestGiver.h"
#include "MVGameMode.h"
#include "Quest.h"
#include "MissionDA.h"
#include "Kismet\GameplayStatics.h"
#include "VV_GameMode.h"
#include "GameController.h"

const UQuest* AMV_QuestGiver::GetQuest() const
{
	return Quest;
}

void AMV_QuestGiver::OnOverlapAction()
{
	GameController->SetStartedQuest(Quest);
	UE_LOG(LogTemp, Error, TEXT("[AMV_QuestGiver].OnOverlapAction Quest Can be Displayed: %s, with Mission: %s"), *Quest->Name.ToString(), *Quest->Mission->Name.ToString());
}
