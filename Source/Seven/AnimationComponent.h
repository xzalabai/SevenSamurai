#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PublicEnums.h"
#include "AnimationComponent.generated.h"

class UAnimInstance;
class ASevenCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEVEN_API UAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAnimationComponent();
	virtual void BeginPlay() override;

private:
	UAnimInstance* GetOwnerAnimInstance();
	ASevenCharacter* GetCharacterOwner();
	bool bActiveMontageRunning = false;
	bool bNextComboTriggerEnabled = false;
	int currentComboIndex = 1;
	int8 currentMontageSection = -1;
	
public:		
	FOnMontageEnded EndDelegate;
	FOnMontageStarted StartDelegate;
	void OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnAnimationStarted(UAnimMontage* Montage);
	int8 GetCurrentAnimationMontageSection();
	void Play(UAnimMontage* AnimMontage, const FName& SectionName, bool bCanInterrupt = false);
	void Play(UAnimMontage* AnimMontage, int SectionName, bool bCanInterrupt);
	UFUNCTION(BlueprintCallable)
	void NextComboTriggered(bool bEnable);
	bool Block(bool bEnable);
	FORCEINLINE bool IsAnimationRunning() const { return bActiveMontageRunning; }

	TArray<AActor*> HitActors;
		
};
