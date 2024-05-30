#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mission.generated.h"

class USphereComponent;

DECLARE_DELEGATE_OneParam(FOnCharacterOverlappedMission, uint32);

UCLASS()
class SEVEN_API AMission : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	uint32 ID;

	UPROPERTY(EditAnywhere)
	uint32 EnemiesCount{ 0 };

	UPROPERTY(EditAnywhere)
	USphereComponent* Area{ nullptr };

	UPROPERTY(EditAnywhere)
	TArray<USceneComponent*> EnemySpawns;

	UPROPERTY(EditAnywhere)
	USceneComponent* SevenCharactersPosition{ nullptr };

	UPROPERTY(EditAnywhere)
	USceneComponent* RootSceneComponent{ nullptr };

	FOnCharacterOverlappedMission OnCharacterOverlappedMission;

public:	
	AMission();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;
};
