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
	
public:	
	AThrowingKnife();
	virtual void Tick(float DeltaTime) override;
	void FireInDirection(const FVector& ShootDirection);

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly);
	USceneComponent* RootSceneComponent;

	UPROPERTY(EditDefaultsOnly);
	UNiagaraComponent* ParticleSystem;

	UPROPERTY(EditDefaultsOnly);
	USphereComponent* TriggerCollider;

	UPROPERTY(EditDefaultsOnly);
	UProjectileMovementComponent* ProjectileMovementComponent;

private:
	FAttackInfo AttackInfo;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FTimerHandle TimerHandle;
	void DestroyActor();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	

};
