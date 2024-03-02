#include "AnimationComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SevenCharacter.h"
#include "AttackComponent.h"
#include "MotionWarpingComponent.h"
#include "Kismet\KismetMathLibrary.h"

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

bool UAnimationComponent::Play(UAnimMontage* AnimMontage, int SectionName, const EMontageType &MontageType, bool bCanInterrupt)
{
	FString myString = FString::FromInt(SectionName);
	return Play(AnimMontage, FName(*myString), MontageType, bCanInterrupt);
}

void UAnimationComponent::WarpAttacker(const FString& WarpName, const ASevenCharacter* Victim)
{
	ASevenCharacter* SevenCharacter = GetCharacterOwner();

	const FVector Direction = Victim->VictimDesiredPosition->GetComponentLocation() - Victim->GetActorLocation();
	const FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(SevenCharacter->GetActorLocation() + SevenCharacter->GetActorForwardVector(), Victim->GetActorLocation());
	const FVector AttackerFinalPosition = Rotation.RotateVector(-Direction) + Victim->GetActorLocation();
	FTransform T(Rotation, AttackerFinalPosition);
	//DrawDebugPoint(GetWorld(), T.GetTranslation(), 15.0f, FColor(0, 0, 255), true);
	SevenCharacter->AC_MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform("MW_LightAttackAttacker", T);
}

bool UAnimationComponent::Play(UAnimMontage* AnimMontage, const FName& SectionName, const EMontageType MontageType, bool bCanInterrupt)
{
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] bCanInterrupt %d, bActiveMontageRunning %d, bNextComboTriggerEnabled %d"),
		(bCanInterrupt ? 1 : 0),
		(bActiveMontageRunning ? 1 : 0),
		(bNextComboTriggerEnabled ? 1 : 0));

	if (MontageType == EMontageType::Attack)
	{
		if (bActiveMontageRunning && bNextComboTriggerEnabled)
		{
			GetCharacterOwner()->StopAnimMontage();
			NextMontageType = EMontageType::Attack;
		}
		else if (bActiveMontageRunning && !bNextComboTriggerEnabled)
		{
			return false;
		}
		else if (!bActiveMontageRunning)
		{

		}
	}
	else
	{
		if (bCanInterrupt && bActiveMontageRunning)
		{
			NextMontageType = MontageType;
			GetCharacterOwner()->StopAnimMontage();
		}
	}

	ASevenCharacter* SevenCharacter = GetCharacterOwner();
	UAnimInstance* AnimInstance = GetOwnerAnimInstance();

	if (!SevenCharacter || !AnimInstance)
	{
		return false;
	}
	
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] Play Animation %s, section:"), *AnimMontage->GetName(), *SectionName.ToString());

	SevenCharacter->PlayAnimMontage(AnimMontage, 1.0f, SectionName);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AnimMontage);
	bActiveMontageRunning = true;
	CurrentMontageType = MontageType;
	return true;
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

void UAnimationComponent::OnEvadeEnded()
{
	GetCharacterOwner()->bIsEvading = false;
}

void UAnimationComponent::NextComboTriggered(bool bEnable)
{
	bNextComboTriggerEnabled = bEnable;
}

bool UAnimationComponent::Block(bool bEnable)
{
	ASevenCharacter* SevenCharacter = GetCharacterOwner();

	if ((bEnable && IsAnimationRunning()) || !SevenCharacter)
		return false;

	SevenCharacter->bIsBlocking = bEnable;
	SevenCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = bEnable;
	SevenCharacter->GetCharacterMovement()->bOrientRotationToMovement = !bEnable;
	SevenCharacter->GetCharacterMovement()->MaxWalkSpeed = bEnable ? 250 : 600;

	return true;
}

FName UAnimationComponent::GetCurrentMontageSection()
{
	if (UAnimInstance* AnimInstance = GetOwnerAnimInstance())
	{
		return AnimInstance->Montage_GetCurrentSection();
	}
	return NAME_None;
}

void UAnimationComponent::OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Warning, TEXT("[UAnimationComponent] OnAnimationEnded. Animation: %s"), *Montage->GetName());
	
	if (!bInterrupted)
	{
		bActiveMontageRunning = false;
		NextMontageType = EMontageType::None;
	}
	
	GetCharacterOwner()->OnAnimationEnded(CurrentMontageType);

	if (NextMontageType == EMontageType::None)
	{
		GetCharacterOwner()->AC_AttackComponent->OnAnimationEnded(CurrentMontageType);
	}
	
	if (CurrentMontageType == EMontageType::Attack)
	{
		GetCharacterOwner()->AttackEnd();
		bNextComboTriggerEnabled = false;
	}

	CurrentMontageType = EMontageType::None;
}

void UAnimationComponent::OnAnimationStarted(UAnimMontage* Montage)
{
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] OnAnimationStarted"));
}
