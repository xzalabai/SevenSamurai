// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RollDice.generated.h"

class ASevenPlayerController;
class UBoxComponent;

UCLASS()
class SEVEN_API ARollDice : public AActor
{
	GENERATED_BODY()
	
public:	
	ARollDice();

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Plane;

	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Dice;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* UpperBound;

	UPROPERTY(EditDefaultsOnly)
	UBoxComponent* LowerBound;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	void Activate();
	void Roll();

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
