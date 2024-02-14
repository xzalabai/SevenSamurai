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
	//StartDelegate.BindUObject(this, &UAnimationComponent::OnAnimationStarted);
	if (ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(GetOwner()))
	{
		if (UAnimInstance* AnimInstance = SevenCharacter->GetMesh()->GetAnimInstance())
		{
			UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] AnimInstance -"));
			AnimInstance->Montage_SetEndDelegate(EndDelegate);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[UAnimationComponent] AnimInstance Not found for %s"), *SevenCharacter->GetName());
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UAnimationComponent] SevenCharacter Not found"));
	}
	
	
}

void UAnimationComponent::Play(UAnimMontage* AnimMontage, const FName& SectionName, bool bCanInterrupt)
{
	if (bCanInterrupt)
	{
		GetCharacterOwner()->StopAnimMontage();
	}
	else if (bActiveMontageRunning && !bNextComboTriggerEnabled)
	{
		return;
	}
	else if (bActiveMontageRunning && bNextComboTriggerEnabled)
	{
		
	}

	ASevenCharacter* SevenCharacter = GetCharacterOwner();
	GetCharacterOwner()->PlayAnimMontage(AnimMontage, 1.0f, FName(*FString::FromInt(currentComboIndex++)) );
	if (UAnimInstance* AnimInstance = GetOwnerAnimInstance())
	{
		AnimInstance->Montage_SetEndDelegate(EndDelegate, AnimMontage);
	}
	
	bActiveMontageRunning = true;
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
		UE_LOG(LogTemp, Error, TEXT("[UAnimationComponent] UAnimInstance AnimInstance Not found"));
		return nullptr;
	}
}

void UAnimationComponent::NextComboTriggered(bool bEnable)
{
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] NextComboTriggered %d"), bEnable ? 1 : 0);
	bNextComboTriggerEnabled = bEnable;
	// TODO REMOVE!
	if (currentComboIndex > 4)
	{
		currentComboIndex = 0;
	}
}

void UAnimationComponent::OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] OnAnimationEnded"));
	bActiveMontageRunning = false;
	bNextComboTriggerEnabled = false;
	GetCharacterOwner()->TargetedEnemy = nullptr;
	//HitActors.Empty();
}

void UAnimationComponent::OnAnimationStarted(UAnimMontage* Montage)
{
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] OnAnimationStarted"));
	//HitActors.Empty();
}
