#pragma once

#include "CoreMinimal.h"
#include "PublicEnums.h"
#include "GameFramework/Actor.h"
#include "Mission.generated.h"

class USphereComponent;
class AEnemyCharacter;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnMissionEnd, bool bPlayerWon);

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
	uint32 SevenCharacterCount{ 0 };

	UPROPERTY(EditAnywhere)
	USphereComponent* Area{ nullptr };

	UPROPERTY(EditAnywhere)
	TArray<USceneComponent*> EnemySpawns;

	UPROPERTY(EditAnywhere)
	USceneComponent* SevenCharactersPosition{ nullptr };

	UPROPERTY(EditAnywhere)
	USceneComponent* RootSceneComponent{ nullptr };

	UPROPERTY(EditAnywhere)
	AMission* SideMission;

	UPROPERTY(EditAnywhere)
	EMissionType MissionType;

	UPROPERTY(EditAnywhere)
	TMap<int, TSubclassOf<AEnemyCharacter>> EnemiesToSpawn;

	UPROPERTY(VisibleAnywhere)
	uint32 EnemyKilledCount{ 0 };

	UPROPERTY(VisibleAnywhere)
	uint32 SevenCharactersKilledCount{ 0 };

	FOnMissionEnd OnMissionEnd;

public:	
	AMission();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void MissionComplete(bool bWin) const;
	void ActivateMission(bool bEnable);
	void MissionStarted() const;
	void MoveAlliesToPlace();
	void OnStatusUpdate(const AActor* Actor, const EEnemyStatus Status);
	FORCEINLINE bool IsSideMission() const { return bSideMission; };
protected:
	virtual void BeginPlay() override;
};
