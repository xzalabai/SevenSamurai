#include "AnimationComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SevenCharacter.h"
#include "SevenPlayerController.h"
#include "AttackComponent.h"
#include "MotionWarpingComponent.h"
#include "Kismet\KismetMathLibrary.h"
#include "Kismet\GameplayStatics.h"

UAnimationComponent::UAnimationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UAnimationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (CachedSevenCharacter && CachedSevenCharacter->CanBePossessed() && CachedSevenCharacter->GetIsGuarding()) // TODO REMOVE FIRST CONDITION!!!
	{
		if (LockedEnemy)
		{
			ASevenPlayerController* PlayerController = CachedSevenCharacter->GetSevenPlayerController(); // TODO: Why I cannot assign directly to APlayerController ??????
			FRotator Rot = UKismetMathLibrary::FindLookAtRotation(CachedSevenCharacter->GetActorLocation(), LockedEnemy->GetActorLocation());
			Rot.Pitch = Rot.Pitch - 25.0f;
			PlayerController->SetControlRotation(Rot);
		}
		else
		{
			Guard(false);
		}
	}
	
	if (IsAttackAnimationRunning() && CachedSevenCharacter && CachedSevenCharacter->TargetedEnemy)
	{
		WarpAttacker(FString(), CachedSevenCharacter->TargetedEnemy);
	}
}

void UAnimationComponent::BeginPlay()
{
	Super::BeginPlay();
	CachedSevenCharacter = GetOwnerCharacter();
	CachedSevenCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	EndDelegate.BindUObject(this, &UAnimationComponent::OnAnimationEnded);
	UAnimInstance* AnimInstance = CachedSevenCharacter->GetMesh()->GetAnimInstance();
	AnimInstance->Montage_SetEndDelegate(EndDelegate);	
}

bool UAnimationComponent::Play(UAnimMontage* AnimMontage, int SectionName, const EMontageType &MontageType, bool bCanInterrupt)
{
	FString myString = FString::FromInt(SectionName);
	return Play(AnimMontage, FName(*myString), MontageType, bCanInterrupt);
}

void UAnimationComponent::PlayAfterCurrent(UAnimMontage* AnimMontage, const FName& SectionName, const EMontageType& MontageType, bool bCanInterrupt)
{
	//AnimationToPlay = FAnimationToPlay(AnimMontage, SectionName, MontageType, bCanInterrupt);
}

void UAnimationComponent::WarpAttacker(const FString& WarpName, const ASevenCharacter* Victim)
{
	// TODO: Understand and FIX this when you will have enough strenght solider
	const FVector Direction = (Victim->VictimDesiredPosition->GetComponentLocation() - Victim->GetActorLocation()) * Victim->GetActorForwardVector().GetSafeNormal();
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Victim->GetActorLocation(), CachedSevenCharacter->GetActorLocation());
	const FVector FinalPosition = Rotation.RotateVector(FVector(Direction.X, 0, 0)) + Victim->GetActorLocation();
	Rotation.Yaw = Rotation.Yaw - 180;
	FTransform T(Rotation, FinalPosition);

	//DrawDebugDirectionalArrow(GetWorld(), FinalPosition, Victim->GetActorLocation(), 12, FColor::Black, true, -1);
	CachedSevenCharacter->AC_MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform("MW_LightAttackAttacker", T);
}

bool UAnimationComponent::Play(UAnimMontage* AnimMontage, const FName& SectionName, const EMontageType MontageType, bool bCanInterrupt)
{
	//UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent].Play %s, bCanInterrupt %d, bActiveMontageRunning %d, bNextComboTriggerEnabled %d MontageType: %d"),
	//	*GetOwner()->GetName(),
	//	(bCanInterrupt ? 1 : 0),
	//	(bActiveMontageRunning ? 1 : 0),
	//	(bNextComboTriggerEnabled ? 1 : 0),
	//	(int)MontageType);

	if (MontageType == EMontageType::LightAttack)
	{
		if (bActiveMontageRunning && bNextComboTriggerEnabled)
		{
			NextMontageType = EMontageType::LightAttack;
			CachedSevenCharacter->StopAnimMontage();
		}
		else if (bActiveMontageRunning && !bNextComboTriggerEnabled)
		{
			if (CurrentMontageType != EMontageType::Parry)
			{
				return false;
			}
		}
		else if (!bActiveMontageRunning)
		{
			NextMontageType = EMontageType::None;
		}
	}
	else
	{
		if (!bCanInterrupt && bActiveMontageRunning)
		{
			return false;
		}

		if (bCanInterrupt && bActiveMontageRunning)
		{
			NextMontageType = MontageType;
			CachedSevenCharacter->StopAnimMontage();
		}
	}

	UAnimInstance* AnimInstance = GetOwnerAnimInstance();

	if (!CachedSevenCharacter || !AnimInstance)
	{
		return false;
	}
	
	//UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] Play Animation %s, section:"), *AnimMontage->GetName(), *SectionName.ToString());

	CachedSevenCharacter->PlayAnimMontage(AnimMontage, 1.0f, SectionName);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AnimMontage);
	bActiveMontageRunning = true;
	CurrentMontageType = MontageType;
	return true;
}

ASevenCharacter* UAnimationComponent::GetOwnerCharacter()
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
	if (UAnimInstance* AnimInstance = CachedSevenCharacter->GetMesh()->GetAnimInstance())
	{
		return AnimInstance;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UAnimationComponent] UAnimInstance AnimInstance Not found"));
		return nullptr;
	}
}

void UAnimationComponent::OnLayingDead()
{
	USkeletalMeshComponent* PlayerMesh = CachedSevenCharacter->GetMesh();
	PlayerMesh->SetCollisionProfileName("Ragdoll");
	PlayerMesh->SetSimulatePhysics(true);
	LockedEnemy = nullptr;
	CachedSevenCharacter->OnLayingDead();
}

void UAnimationComponent::OnEvadeEnded()
{
	CachedSevenCharacter->bIsEvading = false;
}

void UAnimationComponent::NextComboTriggered(bool bEnable)
{
	bNextComboTriggerEnabled = bEnable;
}

bool UAnimationComponent::Block(bool bEnable)
{
	bool &bAttackWasPerformed = bNextComboTriggerEnabled; // We use bNextComboTriggerEnabled also as an indicator of whether attack was already performed.
	
	if (bEnable && IsAnimationRunning())
	{
		if (!bAttackWasPerformed)
		{
			return false;
		}
	}

	CachedSevenCharacter->bIsBlocking = bEnable;
	CachedSevenCharacter->bIsGuarding = false;
	CachedSevenCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = bEnable;
	CachedSevenCharacter->GetCharacterMovement()->bOrientRotationToMovement = !bEnable;
	CachedSevenCharacter->GetCharacterMovement()->MaxWalkSpeed = bEnable ? BlockSpeed : (CachedSevenCharacter->GetIsGuarding() ? GuardSpeed : WalkSpeed);

	return true;
}

bool UAnimationComponent::Guard(bool bEnable)
{
	bool& bAttackWasPerformed = bNextComboTriggerEnabled; // We use bNextComboTriggerEnabled also as an indicator of whether attack was already performed
	if (bEnable && IsAnimationRunning())
	{
		if (bAttackWasPerformed)
		{
			return false;
		}
	}
		
	CachedSevenCharacter->bIsBlocking = false;
	CachedSevenCharacter->bIsGuarding = bEnable;
	CachedSevenCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = bEnable;
	CachedSevenCharacter->GetCharacterMovement()->bOrientRotationToMovement = !bEnable;
	CachedSevenCharacter->GetCharacterMovement()->MaxWalkSpeed = bEnable ? GuardSpeed : WalkSpeed;

	if (bEnable)
	{
		const TArray<ASevenCharacter*> FoundEnemies = CachedSevenCharacter->GetEnemiesInFrontOfCharacer(-1, 200, 1000, 100, true);
		if (FoundEnemies.Num() > 0)
		{
			LockedEnemy = FoundEnemies[0];
		}
	}
	else
	{
		LockedEnemy = nullptr;
	}

	return true;
}

bool UAnimationComponent::IsDefendReactionInProgress() const
{
	if (GetCurrentMontageType() == EMontageType::Block ||
		GetCurrentMontageType() == EMontageType::Evade ||
		GetCurrentMontageType() == EMontageType::Parry)
	{
		return true;
	}
	return false;
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
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] OnAnimationEnded Animation: %s, CurrentMontageType: %d, Character: %s, Interrupted: %d"),
		*Montage->GetName(), (int)CurrentMontageType, *CachedSevenCharacter->GetName(), bInterrupted ? 1 :0);

	const EMontageType StoppedMontage = CurrentMontageType;
	
	if (!bInterrupted)
	{
		bActiveMontageRunning = false;
		NextMontageType = EMontageType::None;
		CurrentMontageType = EMontageType::None;
	}
	else
	{
		CurrentMontageType = NextMontageType;
	}
	
	CachedSevenCharacter->OnAnimationEnded(StoppedMontage, NextMontageType);
	CachedSevenCharacter->AC_AttackComponent->OnAnimationEnded(StoppedMontage, NextMontageType);
	
	if (CurrentMontageType == EMontageType::LightAttack && !bInterrupted)
	{
		//GetOwnerCharacter()->AttackEnd(); // This is called already
		NextComboTriggered(false);
	}
}

void UAnimationComponent::OnAnimationStarted(UAnimMontage* Montage)
{
	//UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] OnAnimationStarted"));
}


