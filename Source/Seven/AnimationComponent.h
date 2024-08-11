#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PublicEnums.h"
#include "AnimationComponent.generated.h"

class UAnimInstance;
class ASevenCharacter;
class ASevenPlayerController;

UENUM(BlueprintType)
enum class EStances : uint8
{
	None,
	Run,
	Block,
	Guard
};

USTRUCT()
struct FMontage
{
	GENERATED_BODY()

	EMontageType MontageType{ EMontageType::None };
	UAnimMontage* AnimMontage{ nullptr };
	EStances LastStance{ EStances::None };

	void Reset()
	{
		MontageType = EMontageType::None;
		AnimMontage = nullptr;
		LastStance = EStances::None;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEVEN_API UAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class UAttackComponent;
	friend class ASevenCharacter;

public:	
	FOnMontageEnded EndDelegate;
	FOnMontageStarted StartDelegate;

protected:
	UPROPERTY(BlueprintReadOnly)
	const ASevenCharacter* LockedEnemy{ nullptr };
	UPROPERTY(BlueprintReadOnly)
	EStances Stance{ EStances::None };

private:
	bool bActiveMontageRunning = false;
	bool bNextComboTriggerEnabled = false;
	UPROPERTY()
	FMontage CurrentMontage{};
	UPROPERTY()
	FMontage LastPlayedMontage{};
	ASevenCharacter* CachedSevenCharacter{ nullptr };
	AController* CachedPlayerController{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int BlockSpeed{ 190 };
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int GuardSpeed{ 150 };
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
	void WarpAttacker(const ASevenCharacter* Victim);
	
	UFUNCTION(BlueprintCallable)
	void OnLayingDead();
	UFUNCTION(BlueprintCallable)
	void OnEvadeEnded(); // TODO: Remove bIsEvading and instead check if montage Evade is still in progress.
	void NextComboTriggered(bool bEnable);
	
	bool Block(const bool bEnable);
	bool Guard(const bool bEnable);
	bool Run(const bool bEnable);
	void LockTarget(bool bEnable, const ASevenCharacter* EnemyToLock = nullptr);
	void SwitchStances(const EStances NewStance);
	FORCEINLINE bool IsAnimationRunning() const { return bActiveMontageRunning; }
	FORCEINLINE bool IsAttackAnimationRunning() const { return (bActiveMontageRunning && AttackMontages.Contains(CurrentMontage.MontageType)); }
	FORCEINLINE EMontageType GetCurrentMontageType() const { return CurrentMontage.MontageType; }
	FName GetCurrentMontageSection();

	TArray<AActor*> HitActors;

private:
	UAnimInstance* GetOwnerAnimInstance();
	ASevenCharacter* GetOwnerCharacter();

};


