// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PublicEnums.h"
#include "AttributesComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SEVEN_API UAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributesComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FORCEINLINE int32 GetHP() const { return AttributesMap[EItemType::HP]; }
	void Set(const EItemType ItemType, const int32 NewHP);
	void Add(const EItemType ItemType, const int32 NewHP);
	const int32& Decrease(const EItemType ItemType, const int32 NewHP);
protected:
	virtual void BeginPlay() override;
	int32 HP = 0;
	TMap<EItemType, int32> AttributesMap;
};
