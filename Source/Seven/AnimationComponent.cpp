#include "AnimationComponent.h"
#include "PublicEnums.h"
#include "GameFramework/CharacterMovementComponent.h"
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

void UAnimationComponent::Play(UAnimMontage* AnimMontage, int SectionName, bool bCanInterrupt)
{
	FString myString = FString::FromInt(SectionName);
	Play(AnimMontage, FName(*myString), bCanInterrupt);
}

void UAnimationComponent::Play(UAnimMontage* AnimMontage, const FName& SectionName, bool bCanInterrupt)
{
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] bCanInterrupt %d, bActiveMontageRunning %d, bNextComboTriggerEnabled %d"),
		(bCanInterrupt ? 1 : 0),
		(bActiveMontageRunning ? 1 : 0),
		(bNextComboTriggerEnabled ? 1 : 0));

	if (bCanInterrupt)
	{
		GetCharacterOwner()->StopAnimMontage();
	}
	else if ((bActiveMontageRunning && !bNextComboTriggerEnabled) || bNextComboTriggered)
	{
		UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] xxx"));
		return;
	}
	else if (bActiveMontageRunning && bNextComboTriggerEnabled && !bNextComboTriggered)
	{
		GetCharacterOwner()->StopAnimMontage();
		bNextComboTriggered = true;
	}

	ASevenCharacter* SevenCharacter = GetCharacterOwner();
	GetCharacterOwner()->PlayAnimMontage(AnimMontage, 1.0f, SectionName);
	if (UAnimInstance* AnimInstance = GetOwnerAnimInstance())
	{
		AnimInstance->Montage_SetEndDelegate(EndDelegate, AnimMontage);
	}
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] Play"));
	bActiveMontageRunning = true;
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
	if (!bEnable)
		bNextComboTriggered = false;
}

bool UAnimationComponent::Block(bool bEnable)
{
	if (IsAnimationRunning())
		return false;

	ASevenCharacter* SevenCharacter = GetCharacterOwner();

	SevenCharacter->bIsBlocking = bEnable;
	SevenCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = bEnable;
	SevenCharacter->GetCharacterMovement()->bOrientRotationToMovement = !bEnable;
	SevenCharacter->GetCharacterMovement()->MaxWalkSpeed = bEnable ? 250 : 600;

	return true;
}

void UAnimationComponent::OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("[UAnimationComponent] OnAnimationEnded"));
	bActiveMontageRunning = false;
	bNextComboTriggerEnabled = false;
	GetCharacterOwner()->OnAnimationEnded();
	//HitActors.Empty();
}

void UAnimationComponent::OnAnimationStarted(UAnimMontage* Montage)
{
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] OnAnimationStarted"));
	//HitActors.Empty();
}

int8 UAnimationComponent::GetCurrentAnimationMontageSection()
{
	return 0;
}
