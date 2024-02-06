#include "AnimationComponent.h"
#include "SevenCharacter.h"

UAnimationComponent::UAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAnimationComponent::BeginPlay()
{
	Super::BeginPlay();
	EndDelegate.BindUObject(this, &UAnimationComponent::OnAnimationEnded);
	if (ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(GetOwner()))
	{
		if (UAnimInstance* AnimInstance = SevenCharacter->GetMesh()->GetAnimInstance())
		{
			UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] AnimInstance -"));
			AnimInstance->Montage_SetEndDelegate(EndDelegate);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[UAnimationComponent] AnimInstance Not found"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UAnimationComponent] SevenCharacter Not found"));
	}
	
	
}

void UAnimationComponent::Play(UAnimMontage* AnimMontage, const FName& SectionName, bool bCanInterrupt)
{
	if (bMontageRunning && !bNextComboTriggerEnabled)
	{
		return;
	}
	if (bMontageRunning && bNextComboTriggerEnabled)
	{
		
	}

	ASevenCharacter* SevenCharacter = GetCharacterOwner();
	GetCharacterOwner()->PlayAnimMontage(AnimMontage, 1.0f, FName(*FString::FromInt(currentComboIndex++)) );
	GetOwnerAnimInstance()->Montage_SetEndDelegate(EndDelegate, SevenCharacter->LightAttack);
	bMontageRunning = true;
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] Play"));
	
}

ASevenCharacter* UAnimationComponent::GetCharacterOwner()
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

UAnimInstance* UAnimationComponent::GetOwnerAnimInstance()
{
	if (UAnimInstance* AnimInstance = GetCharacterOwner()->GetMesh()->GetAnimInstance())
	{
		return AnimInstance;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UAnimationComponent] AnimInstance Not found"));
		return nullptr;
	}
}
void UAnimationComponent::NextComboTriggered(bool bEnable)
{
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] NextComboTriggered %d"), bEnable ? 1 : 0);
	bNextComboTriggerEnabled = bEnable;
}

void UAnimationComponent::OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] OnAnimationEnded"));
	bMontageRunning = false;
	bNextComboTriggerEnabled = false;
}

