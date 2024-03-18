#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PublicEnums.h"
#include "AnimationComponent.generated.h"

class UAnimInstance;
class ASevenCharacter;

USTRUCT()
struct FAnimationToPlay
{
	GENERATED_BODY()
	FAnimationToPlay() = default;
	FAnimationToPlay(UAnimMontage* AnimMontage, const FName SectionName, const EMontageType MontageType, bool bCanInterrupt)
		: AnimMontage(AnimMontage), SectionName(SectionName), MontageType(MontageType), bCanInterrupt(bCanInterrupt) {}
	
	UAnimMontage* AnimMontage = nullptr;
	FName SectionName = FName();
	EMontageType MontageType = EMontageType::None;
	bool bCanInterrupt = false;
	bool IsSet() { return MontageType != EMontageType::None; }
	void Reset()
	{
		AnimMontage = nullptr;
		SectionName = FName();
		MontageType = EMontageType::None;
		bCanInterrupt = false;
	}
		
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEVEN_API UAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAnimationComponent();
	virtual void BeginPlay() override;


private:
	UAnimInstance* GetOwnerAnimInstance();
	ASevenCharacter* GetOwnerCharacter();

	bool bActiveMontageRunning = false;
	bool bNextComboTriggerEnabled = false;
	int8 currentMontageSection = -1;
	EMontageType CurrentMontageType = EMontageType::None;
	EMontageType NextMontageType = EMontageType::None;
	FAnimationToPlay AnimationToPlay;
	
public:		
	FOnMontageEnded EndDelegate;
	FOnMontageStarted StartDelegate;
	void OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnAnimationStarted(UAnimMontage* Montage);
	
	bool Play(UAnimMontage* AnimMontage, const FName& SectionName, const EMontageType MontageType, bool bCanInterrupt = false);
	bool Play(UAnimMontage* AnimMontage, int SectionName, const EMontageType &MontageType, bool bCanInterrupt);
	void PlayAfterCurrent(UAnimMontage* AnimMontage, const FName& SectionName, const EMontageType& MontageType, bool bCanInterrupt);
	void WarpAttacker(const FString& WarpName, const ASevenCharacter* Victim);
	
	UFUNCTION(BlueprintCallable)
	void OnLayingDead();
	UFUNCTION(BlueprintCallable)
	void OnEvadeEnded();
	UFUNCTION(BlueprintCallable)
	void NextComboTriggered(bool bEnable);
	
	bool Block(bool bEnable);
	FORCEINLINE bool IsAnimationRunning() const { return bActiveMontageRunning; }
	FORCEINLINE EMontageType GetCurrentMontageType() const { return CurrentMontageType; }
	FName GetCurrentMontageSection();

	TArray<AActor*> HitActors;

	friend class UAttackComponent;
};


