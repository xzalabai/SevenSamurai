#include "GameController.h"
#include "SevenCharacter.h"
#include "SevenPlayerController.h"
#include "MissionDA.h"
#include "AttributesComponent.h"
#include "Kismet\GameplayStatics.h"
#include "Mission.h"

void UGameController::Initialize(FSubsystemCollectionBase& Collection)
{
	UniqueIDCounter = 0;
	SelectedCharacters.Empty();
}

void UGameController::SetActiveMission(const UMissionDA* Mission)
{
	ActiveMission = Mission;
	UGameplayStatics::OpenLevel(this, FName("ThirdPersonMap"));
}

void UGameController::AddToSelectedCharacter(USevenCharacterDA* SevenCharacterDA)
{
	SelectedCharacters.Add(SevenCharacterDA);
}

const TArray<USevenCharacterDA*> UGameController::GetSelectedCharacters() const
{
	return SelectedCharacters;
}
