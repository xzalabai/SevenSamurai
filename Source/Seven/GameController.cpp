#include "GameController.h"
#include "Kismet\GameplayStatics.h"

void UGameController::Restart()
{
	//OnRestart.Broadcast(); 

	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}
