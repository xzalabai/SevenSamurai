#include "VV_Map.h"
#include "GameController.h"
#include "PaperSpriteActor.h"
#include "PaperSpriteComponent.h"
#include "Components/CapsuleComponent.h"
#include "MVSevenCharacter.h"
#include <Kismet\KismetMathLibrary.h>
#include <Kismet\GameplayStatics.h>

AVV_Map::AVV_Map()
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Exit Component"));
	CapsuleComponent->SetupAttachment(RootComponent);
}

void AVV_Map::BeginPlay()
{
	MVSevenCharacter = Cast<AMVSevenCharacter>(UGameplayStatics::GetActorOfClass(this, AMVSevenCharacter::StaticClass()));
	UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	VillageID = GameController->GetVisitedVillageID();
}

void AVV_Map::OnOverlapAction()
{
	ExitVillage();
}

void AVV_Map::ExitVillage() const
{
	UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	GameController->ExitVillage(VillageID);
}

const TObjectPtr<AMVSevenCharacter> AVV_Map::GetMVSevenCharacter() const
{
	return MVSevenCharacter;
}
