#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Mission.generated.h"

class USphereComponent;
class AEnemyCharacter;

DECLARE_DELEGATE_OneParam(FOnCharacterOverlappedMission, uint32);

UENUM(BlueprintType)
enum class EMissionType : uint8
{
	NotProvided,
	LiberatePlace,
	TakeAmbush,
	ReceiveAmbush,
};


UCLASS()
class SEVEN_API AMission : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	uint32 ID;

	UPROPERTY(EditAnywhere)
	bool bSideMission{ false };

	UPROPERTY(EditAnywhere)
	FName MissionName;

	UPROPERTY(EditAnywhere)
	FName Description;

	UPROPERTY(EditAnywhere)
	uint32 EnemiesCount{ 0 };

	UPROPERTY(EditAnywhere)
	uint32 SevenCharacterCount{ 0 };

	UPROPERTY(EditAnywhere)
	USphereComponent* Area{ nullptr };

	UPROPERTY(EditAnywhere)
	TArray<USceneComponent*> EnemySpawns;

	UPROPERTY(EditAnywhere)
	USceneComponent* SevenCharactersPosition{ nullptr };

	UPROPERTY(EditAnywhere)
	USceneComponent* RootSceneComponent{ nullptr };

	FOnCharacterOverlappedMission OnCharacterOverlappedMission;

	UPROPERTY(EditAnywhere)
	AMission* SideMission;

	UPROPERTY(EditAnywhere)
	EMissionType MissionType;

	UPROPERTY(EditAnywhere)
	TMap<int, TSubclassOf<AEnemyCharacter>> EnemiesToSpawn;


public:	
	AMission();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void MissionComplete(bool bWin) const;
	void ActivateMission(bool bEnable);
	void MissionStarted() const;
protected:
	virtual void BeginPlay() override;
};
