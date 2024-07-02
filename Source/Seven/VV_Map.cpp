#include "VV_Map.h"
#include "GameController.h"
#include "PaperSpriteActor.h"
#include "PaperSpriteComponent.h"
#include "MVSevenCharacter.h"
#include <Kismet\KismetMathLibrary.h>
#include <Kismet\GameplayStatics.h>

void AVV_Map::BeginPlay()
{
	MVSevenCharacter = Cast<AMVSevenCharacter>(UGameplayStatics::GetActorOfClass(this, AMVSevenCharacter::StaticClass()));
}

TObjectPtr<AMVSevenCharacter> AVV_Map::GetMVSevenCharacter() const
{
	return MVSevenCharacter;
}
