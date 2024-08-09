#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"

#include "Shield.generated.h"

class UBoxComponent;
class ASevenCharacter;

UCLASS()
class SEVEN_API AShield : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UBoxComponent* BlockCollider;

	UPROPERTY()
	ASevenCharacter* CachedSevenCharacter;

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	AShield();
	void AttachToSocket(USkeletalMeshComponent* PlayerMesh, FName SocketName);
	void EnableShieldHits(bool bEnable);
};
