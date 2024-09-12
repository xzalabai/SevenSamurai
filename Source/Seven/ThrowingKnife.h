#pragma once

#include "CoreMinimal.h"
#include "PublicEnums.h"
#include "GameFramework/Actor.h"
#include "ThrowingKnife.generated.h"

class UNiagaraComponent;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class SEVEN_API AThrowingKnife : public AActor
{
	GENERATED_BODY()

	friend class UKnifeThrowCombo;
	
public:	
	AThrowingKnife();
	virtual void Tick(float DeltaTime) override;
	void FireInDirection(const FVector& ShootDirection);

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly);
	UNiagaraComponent* ParticleSystem;

	UPROPERTY(EditDefaultsOnly);
	USphereComponent* TriggerCollider;

	UPROPERTY(EditDefaultsOnly);
	UProjectileMovementComponent* ProjectileMovementComponent;

private:
	FAttackInfo AttackInfo;
	FTimerHandle TimerHandle;
	FTimerHandle CollissionsTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void DestroyActor();
	void EnableCollissions();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	

};
