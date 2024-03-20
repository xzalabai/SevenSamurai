// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributesComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SEVEN_API UAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributesComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	const int32& DecreaseHP(const int32 Decrease);
	FORCEINLINE int32 GetHP() const { return HP; }
	void SetHP(const int32 NewHP);
protected:
	virtual void BeginPlay() override;
	int32 HP = 0;
		
};
