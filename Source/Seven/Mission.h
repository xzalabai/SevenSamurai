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
	USphereComponent* Area;

	UPROPERTY(EditAnywhere)
	USceneComponent* EnemySpawn;

	UPROPERTY(EditAnywhere)
	USceneComponent* SevenCharactersPosition;

	UPROPERTY(EditAnywhere)
	USceneComponent* RootSceneComponent;

	FOnCharacterOverlappedMission OnCharacterOverlappedMission;

public:	
	AMission();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;
};
