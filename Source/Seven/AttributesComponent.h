// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PublicEnums.h"
#include "AttributesComponent.generated.h"

class ASevenCharacter;
class ASevenPlayerController;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SEVEN_API UAttributesComponent : public UActorComponent
{
	friend class GameController;

	GENERATED_BODY()

private:
	TMap<EItemType, float> AttributesMap;
	TMap<EItemType, float> MaxAttributesMap;
	ASevenPlayerController* SevenPlayerController{ nullptr };
	bool bIsPlayer{ false };
	
public:	
	UAttributesComponent();	
	FORCEINLINE float GetHP() const { return AttributesMap[EItemType::HP]; }
	FORCEINLINE float GetXP() const { return AttributesMap[EItemType::XP]; }
	void Set(const EItemType ItemType, const float Amount);
	void Add(const EItemType ItemType, const float Amount);
	uint16 Decrease(const EItemType ItemType, const float Decrease);
	void OnPossessed() const;
protected:
	virtual void BeginPlay() override;
private:
	ASevenCharacter* GetOwnerCharacter();
	void NotifyUI(const EItemType ChangedItemType) const;
};
