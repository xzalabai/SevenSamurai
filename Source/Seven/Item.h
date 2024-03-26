#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PublicEnums.h"
#include "Item.generated.h"

class USphereComponent;

UCLASS()
class SEVEN_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();
	virtual void Tick(float DeltaTime) override;
	void Init(const EItemType ItemType, const int Amount);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	EItemType ItemType;

	UPROPERTY(EditDefaultsOnly);
	USphereComponent* TriggerCollider;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly)
	int Amount = 0;

};
