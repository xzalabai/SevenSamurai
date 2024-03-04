#pragma once

#include "CoreMinimal.h"
#include "PublicEnums.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UNiagaraComponent;
class USphereComponent;
class ASevenCharacter;
class USkeletalMeshComponent;

UCLASS()
class SEVEN_API AWeapon : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;
	
	UPROPERTY(EditAnywhere);
	USceneComponent* RootSceneComponent;
	
	UPROPERTY(EditAnywhere);
	UNiagaraComponent* ParticleSystem;
	
	//UPROPERTY(VisibleAnywhere);
	//USphereComponent* TriggerCollider;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* StartTrace;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* EndTrace;

	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> HitActors;

	UPROPERTY()
	ASevenCharacter* OwnerCharacter;

private:
	// Cached data for Weapon Trace
	TArray<FHitResult> OutHit;
	TArray<AActor*> ActorsToIgnore;
	FAttackInfo AttackInfo;

public:	
	AWeapon();
	void AttachToSocket(USkeletalMeshComponent* PlayerMesh, FName SocketName);
	UFUNCTION(BlueprintCallable)
	void PerformTrace();
	void AttackStart();
};	
