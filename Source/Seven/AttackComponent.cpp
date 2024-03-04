#include "AttackComponent.h"
#include "AnimationComponent.h"
#include "MotionWarpingComponent.h"
#include "SevenCharacter.h"

UAttackComponent::UAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

const TPair<UAnimMontage*, FName> UAttackComponent::GetAttackMontageToBePlayed()
{
	if (CurrentAttackType != EAttackType::Heavy && CurrentAttackType != EAttackType::Light)
	{
		UE_LOG(LogTemp, Error, TEXT("[UAttackComponent]GetAttackMontageToBePlayed.CurrentAttackType == %d "), (int)CurrentAttackType);
		return TPair<UAnimMontage*, FName>(nullptr, FName());
	}

	const ASevenCharacter* SevenCharacter = GetOwnerCharacter();
	UAnimMontage* MontageToBePlayed = CurrentAttackType == EAttackType::Light ? GetOwnerCharacter()->LightAttackAttacker : GetOwnerCharacter()->HeavyAttackAttacker;
	
	if (SevenCharacter->AC_Animation->GetCurrentMontageSection() == NAME_None)
	{
		// No Animation in progress
		CurrentSection = 1;
	}
	if (SevenCharacter->AC_Animation->GetCurrentMontageType() == EMontageType::Attack)
	{
		// Some Attack animation is in progress
		CurrentSection = CustomMath::FNameToInt(SevenCharacter->AC_Animation->GetCurrentMontageSection());
		if (CurrentSection < MontageToBePlayed->CompositeSections.Num())
		{
			// We can still iterate
			++CurrentSection;
		}
		else
		{
			CurrentSection = 1;
			// TODO: HERE SHOULD BE COOLDOWN!
		}
		
	}
	// Maybe removeWarp should be here!!!!!!!!!!!
	//CurrentAttackType = AttackType;
	const FName SectionToPlay = CustomMath::IntToFName(CurrentSection);
	return TPair< UAnimMontage*, FName> (MontageToBePlayed, SectionToPlay);
}

void UAttackComponent::OnAnimationEnded(const EMontageType &StoppedMontage, const EMontageType &NextMontage)
{
	if (StoppedMontage == EMontageType::Attack && NextMontage != EMontageType::Attack)
	{
		if (CurrentAttackType == EAttackType::Heavy)
		{
			bHeavyAttackReady = false;
			bHeavyAttackWasReleased = false;
		}
	
		CurrentSection = 1;
		CurrentAttackType = EAttackType::None;		
	}
}

FAttackInfo UAttackComponent::GetInfoAboutAttack() const
{
	return FAttackInfo(CurrentAttackType, 0, CurrentAttackType == EAttackType::Light ? 10 : 20, GetOwner());
}

bool UAttackComponent::PlayAttack(ASevenCharacter* TargetedEnemy, bool bWarp, bool canInterrupt)
{
	const TPair<UAnimMontage*, FName> NextAttack = GetAttackMontageToBePlayed();
	
	if (!NextAttack.Key)
	{
		UE_LOG(LogTemp, Error, TEXT("[UAttackComponent]HeavyAttack.NextAttack.Key is nullptr"));
		return false;
	}

	if (GetOwnerCharacter()->AC_Animation->Play(NextAttack.Key, NextAttack.Value, EMontageType::Attack, canInterrupt))
	{
		if (TargetedEnemy && bWarp)
		{
			GetOwnerCharacter()->AC_Animation->WarpAttacker("MW_LightAttackAttacker", TargetedEnemy);
			UE_LOG(LogTemp, Display, TEXT("[UAttackComponent]LightAttack.TargetedEnemy %s"), *TargetedEnemy->GetName());
		}
	}
	else
	{
		return false;
	}
	return true;
}

void UAttackComponent::LightAttack(ASevenCharacter* TargetedEnemy)
{
	CurrentAttackType = EAttackType::Light;
	if (TargetedEnemy)
	{
		// Targeted Attack
		PlayAttack(TargetedEnemy, true, false);
	}
	else
	{
		// Attack to emptyness
		PlayAttack(nullptr, false, true);
	}	
}

void UAttackComponent::HeavyAttack(ASevenCharacter* TargetedEnemy, const bool bReleased)
{
	if (CurrentAttackType != EAttackType::Heavy && bReleased)
	{
		// Was holding mouse and there was not attack
		return;
	}
	else if (CurrentAttackType != EAttackType::Heavy)
	{
		// Initialization phase of attack
		CurrentAttackType = EAttackType::Heavy;
		PlayAttack(nullptr, false, true);
	}
	else if (bReleased && !bHeavyAttackReady)
	{
		// Released too soon, wait until Attack is Ready (punishment for Heavy attack)
		bHeavyAttackWasReleased = true;
		return;
	}
	else if (bHeavyAttackReady && bReleased)
	{
		// Released during Idle, play attack
		PlayAttack(TargetedEnemy, true, true);
		bHeavyAttackReady = false;
		GetOwnerCharacter()->AttackStart(); // Will be needed in case of accumulating damage while in ready pose
	}
}

void UAttackComponent::SetIsHeavyAttackReady(bool bEnable)
{
	if (bHeavyAttackWasReleased)
	{
		// If we already released mouse, perform attack
		PlayAttack(nullptr, false, true);
	}
	bHeavyAttackReady = true;
}

ASevenCharacter* UAttackComponent::GetOwnerCharacter()
{
	if (ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(GetOwner()))
	{
		return SevenCharacter;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UAnimationComponent] SevenCharacter Not found"));
		return nullptr;
	}
}

UAnimInstance* UAttackComponent::GetOwnerAnimInstance()
{
	if (UAnimInstance* AnimInstance = GetOwnerCharacter()->GetMesh()->GetAnimInstance())
	{
		return AnimInstance;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UAnimationComponent] UAnimInstance AnimInstance Not found"));
		return nullptr;
	}
}

