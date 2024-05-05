#pragma once

#include "CoreMinimal.h"
#include "PublicEnums.h"
#include "Components/ActorComponent.h"
#include "LootGenerator.generated.h"

class AItem;

UCLASS(ClassGroup = (Custom), BlueprintType, Blueprintable, meta = (BlueprintSpawnableComponent))
class SEVEN_API ULootGenerator : public UActorComponent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AItem> GoldItemSubclass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AItem> RiceItemSubclass;

public:	
	ULootGenerator();

protected:
	virtual void BeginPlay() override;
	void OnCharacterKilled(const AActor* Actor, const EEnemyStatus Status);
	const TPair<EItemType, int> GenerateLootDrop() const;
		
};
