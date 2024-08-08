#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PublicEnums.h"
#include "AnimationComponent.generated.h"

class UAnimInstance;
class ASevenCharacter;

USTRUCT()
struct FMontage
{
	GENERATED_BODY()

	EMontageType MontageType{ EMontageType::None };
	UAnimMontage* AnimMontage{ nullptr };

	void Reset()
	{
		MontageType = EMontageType::None;
		AnimMontage = nullptr;
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
	UPROPERTY()
	FMontage CurrentMontage{};
	UPROPERTY()
	FMontage LastPlayedMontage{};
	ASevenCharacter* CachedSevenCharacter{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int BlockSpeed{ 190 };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int GuardSpeed{ 170 };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int WalkSpeed{ 600 };

	
public:		
	UAnimationComponent();
	virtual void BeginPlay() override;
	void OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted);
	void OnAnimationStarted(UAnimMontage* Montage);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	bool Play(UAnimMontage* AnimMontage, const FName& SectionName, const EMontageType MontageType);
	bool Play(UAnimMontage* AnimMontage, int SectionName, const EMontageType &MontageType);
	bool CanPlayAnimation(const EMontageType MontageType) const;
	void WarpAttacker(const FString& WarpName, const ASevenCharacter* Victim);
	
	UFUNCTION(BlueprintCallable)
	void OnLayingDead();
	UFUNCTION(BlueprintCallable)
	void OnEvadeEnded();
	void NextComboTriggered(bool bEnable);
	
	bool Block(bool bEnable);
	bool Guard(bool bEnable);
	bool IsDefendReactionInProgress() const;
	FORCEINLINE bool IsAnimationRunning() const { return bActiveMontageRunning; }
	FORCEINLINE bool IsAttackAnimationRunning() const { return (bActiveMontageRunning && AttackMontages.Contains(CurrentMontage.MontageType)); }
	FORCEINLINE EMontageType GetCurrentMontageType() const { return CurrentMontage.MontageType; }
	FName GetCurrentMontageSection();

	TArray<AActor*> HitActors;

private:
	UAnimInstance* GetOwnerAnimInstance();
	ASevenCharacter* GetOwnerCharacter();

};


