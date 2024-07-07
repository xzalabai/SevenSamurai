#include "VV_GameMode.h"
#include "Quest.h"
#include "MV_Map.h"
#include "MV_EntityBase.h"
#include "Kismet\GameplayStatics.h"


const UQuest* AVV_GameMode::GenerateQuest() const
{
	check(1 == 0);
	AMV_Map* const Map = Cast<AMV_Map>(UGameplayStatics::GetActorOfClass(this, AMV_Map::StaticClass()));
	//const AMV_EntityBase* NewEntity = Map->GenerateEntity(-1, EMissionType::TakeAmbush);
	//FString NewName = "This is your mission quest: " + NewEntity->GetMissionDA()->GetName();

	//UQuest* Quest = NewObject<UQuest>();

	//Quest->Name = FName(*NewName);
	//Quest->Mission = NewEntity->GetMissionDA();

	//return Quest;
	return nullptr;
}