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
	friend class ASevenCharacter;

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
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

public:	
	AShield();
	void AttachToSocket(USkeletalMeshComponent* PlayerMesh, FName SocketName);
	void EnableShieldHits(bool bEnable);
};
