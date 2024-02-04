// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

class UNiagaraComponent;
class USphereComponent;

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
	
	UPROPERTY(VisibleAnywhere);
	USphereComponent* TriggerCollider;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* StartTrace;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* EndTrace;
public:	
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void PerformTrace();

protected:
	virtual void BeginPlay() override;
};
