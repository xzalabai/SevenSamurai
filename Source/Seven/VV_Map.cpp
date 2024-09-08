#include "VV_Map.h"
#include "GameController.h"
#include "PaperSpriteActor.h"
#include "PaperSpriteComponent.h"
#include "Components/CapsuleComponent.h"
#include "MVSevenCharacter.h"
#include "VV_CharacterShop.h"
#include "VV_CharacterUpgrade.h"
#include <Kismet\KismetMathLibrary.h>
#include <Kismet\GameplayStatics.h>

AVV_Map::AVV_Map()
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Exit Component"));
	CapsuleComponent->SetupAttachment(RootComponent);
}

void AVV_Map::BeginPlay()
{
	UE_LOG(LogTemp, Display, TEXT("[AVV_Map].BeginPlay"));

	MVSevenCharacter = Cast<AMVSevenCharacter>(UGameplayStatics::GetActorOfClass(this, AMVSevenCharacter::StaticClass()));
	UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	VillageID = GameController->GetVisitedVillageID();
	const FName VillageIDFname = CustomMath::IntToFName(VillageID);

	if (FVillageProperties* RetrievedVillage = MV_Village->FindRow<FVillageProperties>(VillageIDFname, FString(), true))
	{
		CharacterShop->AvailableCharacters = RetrievedVillage->AvailableCharacters;
		CharacterUpgrade->AvailableCombos = RetrievedVillage->AvailableCombos;
	}
	else
	{
		const TArray<USevenCharacterDA*> AvailableCharactersInVillage = CharacterShop->GenerateAvailableCharacters();
		const TArray<FComboWithPrice>& AvailableCombosInVillage = CharacterUpgrade->GenerateAvailableCombos(GameController->GetSelectedCharacters());
		MV_Village->AddRow(VillageIDFname, FVillageProperties{ GameController->GetVisitedVillageID(), AvailableCombosInVillage, AvailableCharactersInVillage });
	}
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
