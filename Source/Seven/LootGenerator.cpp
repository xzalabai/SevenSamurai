#include "LootGenerator.h"
#include "SevenPlayerController.h"
#include "Kismet\GameplayStatics.h"
#include "Item.h"

ULootGenerator::ULootGenerator()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void ULootGenerator::BeginPlay()
{
	Super::BeginPlay();
	TObjectPtr<ASevenPlayerController> SevenPlayerController = Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	SevenPlayerController->OnUpdateStatus.AddUObject(this, &ULootGenerator::OnCharacterKilled);
}

void ULootGenerator::OnCharacterKilled(const AActor* Actor, const EEnemyStatus Status)
{
	if (Status != EEnemyStatus::Dead || !GoldItemSubclass)
	{
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("[ULootGenerator] ONCharacter"));

	FTransform T{ Actor->GetActorLocation() };
	AItem* ItemToBeSpawned = GetWorld()->SpawnActorDeferred<AItem>(GoldItemSubclass, T);
	
	// TODO: make magic here, for now just random values:
	
	ItemToBeSpawned->Init(EItemType::Gold, 10);
	ItemToBeSpawned->FinishSpawning(T);
}

