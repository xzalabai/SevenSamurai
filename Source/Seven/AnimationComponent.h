#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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
		
public:		
	FOnMontageEnded EndDelegate;
	void OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted);
	void Play(UAnimMontage* AnimMontage, const FName& SectionName, bool bCanInterrupt = false);
	bool bMontageRunning = false;
	bool bNextComboTriggerEnabled = false;
	int currentComboIndex = 1;

	UFUNCTION(BlueprintCallable)
	void NextComboTriggered(bool bEnable);
		
};
