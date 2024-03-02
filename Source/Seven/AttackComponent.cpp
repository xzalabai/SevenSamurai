#include "AttackComponent.h"
#include "AnimationComponent.h"
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
	ASevenCharacter* SevenCharacter = GetCharacterOwner();
	
	if (SevenCharacter->AC_Animation->GetCurrentMontageSection() == NAME_None)
	{
		// No Animation in progress
		CurrentSection = 1;
	}
	if (SevenCharacter->AC_Animation->GetCurrentMontageType() == EMontageType::Attack)
	{
		// Some Attack animation is in progress
		CurrentSection = CustomMath::FNameToInt(SevenCharacter->AC_Animation->GetCurrentMontageSection());
		if (CurrentSection < GetCharacterOwner()->LightAttackAttacker->CompositeSections.Num())
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

	const FName SectionToPlay = CustomMath::IntToFName(CurrentSection);
	UAnimMontage* MontageToPlay = GetCharacterOwner()->LightAttackAttacker;
	return TPair< UAnimMontage*, FName> (MontageToPlay, SectionToPlay);
}

void UAttackComponent::OnAnimationEnded(const EMontageType &StoppedMontage, const EMontageType &NextMontage)
{
	if (StoppedMontage == EMontageType::Attack && NextMontage != EMontageType::Attack)
	{
		CurrentSection = 1;
	}
}

ASevenCharacter* UAttackComponent::GetCharacterOwner()
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
	if (UAnimInstance* AnimInstance = GetCharacterOwner()->GetMesh()->GetAnimInstance())
	{
		return AnimInstance;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UAnimationComponent] UAnimInstance AnimInstance Not found"));
		return nullptr;
	}
}

