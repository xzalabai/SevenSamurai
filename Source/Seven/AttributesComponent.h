// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PublicEnums.h"
#include "AttributesComponent.generated.h"

class ASevenCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SEVEN_API UAttributesComponent : public UActorComponent
{
	friend class GameController;

	GENERATED_BODY()

private:
	TMap<EItemType, uint16> AttributesMap;
	TMap<EItemType, uint16> MaxAttributesMap;
	
public:	
	UAttributesComponent();	
	FORCEINLINE uint16 GetHP() const { return AttributesMap[EItemType::HP]; }
	FORCEINLINE uint16 GetXP() const { return AttributesMap[EItemType::XP]; }
	void Set(const EItemType ItemType, const uint16 Amount);
	void Add(const EItemType ItemType, const uint16 Amount);
	const uint16& Decrease(const EItemType ItemType, const int32 Decrease);
protected:
	virtual void BeginPlay() override;
private:
	ASevenCharacter* GetOwnerCharacter();
};
