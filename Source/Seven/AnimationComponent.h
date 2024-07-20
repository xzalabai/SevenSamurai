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

	friend class UAttackComponent;

public:	
	FOnMontageEnded EndDelegate;
	FOnMontageStarted StartDelegate;

protected:
	UPROPERTY(BlueprintReadOnly)
	ASevenCharacter* LockedEnemy{ nullptr };

private:
	bool bActiveMontageRunning = false;
	bool bNextComboTriggerEnabled = false;
	int8 currentMontageSection = -1;
	EMontageType CurrentMontageType = EMontageType::None;
	EMontageType NextMontageType = EMontageType::None;
	FAnimationToPlay AnimationToPlay;
	
public:		
	UAnimationComponent();
	virtual void BeginPlay() override;
	void OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnAnimationStarted(UAnimMontage* Montage);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	bool Play(UAnimMontage* AnimMontage, const FName& SectionName, const EMontageType MontageType, bool bCanInterrupt = false);
	bool Play(UAnimMontage* AnimMontage, int SectionName, const EMontageType &MontageType, bool bCanInterrupt);
	void PlayAfterCurrent(UAnimMontage* AnimMontage, const FName& SectionName, const EMontageType& MontageType, bool bCanInterrupt);
	void WarpAttacker(const FString& WarpName, const ASevenCharacter* Victim);
	
	UFUNCTION(BlueprintCallable)
	void OnLayingDead();
	UFUNCTION(BlueprintCallable)
	void OnEvadeEnded();
	void NextComboTriggered(bool bEnable);
	
	bool Block(bool bEnable);
	bool Guard(bool bEnable);
	FORCEINLINE bool IsAnimationRunning() const { return bActiveMontageRunning; }
	FORCEINLINE EMontageType GetCurrentMontageType() const { return CurrentMontageType; }
	FName GetCurrentMontageSection();

	TArray<AActor*> HitActors;

private:
	UAnimInstance* GetOwnerAnimInstance();
	ASevenCharacter* GetOwnerCharacter();

};


