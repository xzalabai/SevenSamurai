#pragma once

#include "CoreMinimal.h"
#include "PublicEnums.h"
#include "GameFramework/Actor.h"
#include "Mission.generated.h"

class USphereComponent;
class UMissionDA;
class ASevenCharacter;
class AEnemyCharacter;
class ASevenGameMode;

UENUM(BlueprintType)
enum class EMissionType : uint8
{
	NotProvided UMETA(DisplayName = "NotProvided"),
	LiberatePlace UMETA(DisplayName = "LiberatePlace"),
	TakeAmbush UMETA(DisplayName = "TakeAmbush"),
	ReceiveAmbush UMETA(DisplayName = "ReceiveAmbush"),
	EnemyCamp UMETA(DisplayName = "EnemyCamp"),
	Enemy UMETA(DisplayName = "Enemy"),
	EasyQuest UMETA(DisplayName = "EasyQuest"), 
	AbandonedRuins UMETA(DisplayName = "AbandonedRuins"), // KEEP THIS LAST!
};

static FName MissionTypeToFName(EMissionType MissionType)
{
	return *UEnum::GetDisplayValueAsText(MissionType).ToString();
}


UCLASS()
class SEVEN_API AMission : public AActor
{
	GENERATED_BODY()
	
	friend class SevenGameMode;

public:
	UPROPERTY(EditAnywhere)
	uint32 ID;

	UPROPERTY()
	UMissionDA* MissionDA{ nullptr };

	UPROPERTY(EditAnywhere)
	USphereComponent* Area{ nullptr };

	UPROPERTY(EditAnywhere)
	TArray<USceneComponent*> EnemySpawns;

	UPROPERTY(EditAnywhere)
	USceneComponent* SevenCharactersPosition{ nullptr };

	UPROPERTY(EditAnywhere)
	USceneComponent* RootSceneComponent{ nullptr };

	UPROPERTY(EditAnywhere)
	mutable uint32 EnemyCount{ 0 };

	UPROPERTY(EditAnywhere)
	uint32 SevenCharacterCount{ 0 };

	UPROPERTY(VisibleAnywhere)
	uint32 EnemyKilledCount{ 0 };

	UPROPERTY(VisibleAnywhere)
	uint32 SevenCharactersKilledCount{ 0 };

	UPROPERTY()
	ASevenGameMode* CachedSevenGameMode{ nullptr };

public:	
	AMission();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void ActivateMission(bool bEnable);
	void MissionStarted();
	void SpawnSevenCharacter() const;
	void SpawnEnemies() const;
	void MoveAlliesToPlace();
	void OnStatusUpdate(const ASevenCharacter* SevenCharacter, const ECharacterState Status);
protected:
	virtual void BeginPlay() override;
};
