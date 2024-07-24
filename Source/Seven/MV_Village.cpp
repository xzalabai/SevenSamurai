#include "MV_Village.h"
#include "MissionDA.h"
#include "GameController.h"

void AMV_Village::OnOverlapAction()
{
	UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	if (MissionDA->MissionStatus != EStatus::Completed)
	{
		GameController->SetStartedEntity(this, MissionDA);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UGameController].OnOverlapAction "));
		GameController->EnterVillage(MissionDA->UniqueID);
	}
}

