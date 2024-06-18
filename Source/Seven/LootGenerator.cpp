#include "LootGenerator.h"
#include "SevenPlayerController.h"
#include "SevenCharacter.h"
#include "Kismet\GameplayStatics.h"
#include "Item.h"
#include "GameController.h"

ULootGenerator::ULootGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void ULootGenerator::BeginPlay()
{
	Super::BeginPlay();

	const UGameInstance* GameInstance = Cast<UGameInstance>(GetWorld()->GetGameInstance());
	UGameController* GameController = Cast<UGameController>(GameInstance->GetSubsystem<UGameController>());
	GameController->OnStatusUpdate.AddUObject(this, &ULootGenerator::OnCharacterKilled);

	check(GoldItemSubclass != nullptr);
	check(RiceItemSubclass != nullptr);
}

void ULootGenerator::OnCharacterKilled(const AActor* Actor, const EEnemyStatus Status)
{
	const ASevenCharacter* KilledCharacter = Cast<ASevenCharacter>(Actor);

	if (Status != EEnemyStatus::Dead || !KilledCharacter->IsEnemy())
	{
		return;
	}

	const TPair<EItemType, int> Loot = GenerateLootDrop();
	TSubclassOf<AItem> SubclassToBeSpawned;
	switch (Loot.Key)
	{
		case EItemType::Rice:
			SubclassToBeSpawned = RiceItemSubclass;
			break;
		case EItemType::Gold:
			SubclassToBeSpawned = GoldItemSubclass;
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("[ULootGenerator]OnCharacterKilled Wrong Item to be spawned"));
			break;
	}

	check(SubclassToBeSpawned != nullptr);

	FTransform T{ KilledCharacter->GetActorLocation() };
	AItem* ItemToBeSpawned = GetWorld()->SpawnActorDeferred<AItem>(SubclassToBeSpawned, T);
	
	ItemToBeSpawned->Init(Loot.Key, Loot.Value);
	ItemToBeSpawned->FinishSpawning(T);
}

const TPair<EItemType, int> ULootGenerator::GenerateLootDrop() const
{
	// TODO:Magic

	TPair<EItemType, int> Loot{EItemType::Rice, 1};
	return Loot;
}



