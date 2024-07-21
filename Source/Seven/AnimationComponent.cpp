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
	if (const ASevenCharacter* SevenCharacter = GetOwnerCharacter())
	{
		if (SevenCharacter->CanBePossessed() && SevenCharacter->GetIsGuarding()) // TODO REMOVE FIRST CONDITION!!!
		{
			if (LockedEnemy)
			{
				ASevenPlayerController* PlayerController = SevenCharacter->GetSevenPlayerController(); // TODO: Why I cannot assign directly to APlayerController ??????
				FRotator Rot = UKismetMathLibrary::FindLookAtRotation(SevenCharacter->GetActorLocation(), LockedEnemy->GetActorLocation());
				Rot.Pitch = Rot.Pitch - 25.0f;
				PlayerController->SetControlRotation(Rot);
			}
			else
			{
				Guard(false);
			}
		}
		
	}
}

void UAnimationComponent::BeginPlay()
{
	Super::BeginPlay();
	EndDelegate.BindUObject(this, &UAnimationComponent::OnAnimationEnded);

	if (ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(GetOwner()))
	{
		if (UAnimInstance* AnimInstance = SevenCharacter->GetMesh()->GetAnimInstance())
		{
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

void UAnimationComponent::PlayAfterCurrent(UAnimMontage* AnimMontage, const FName& SectionName, const EMontageType& MontageType, bool bCanInterrupt)
{
	//AnimationToPlay = FAnimationToPlay(AnimMontage, SectionName, MontageType, bCanInterrupt);
}

void UAnimationComponent::WarpAttacker(const FString& WarpName, const ASevenCharacter* Victim)
{
	ASevenCharacter* SevenCharacter = GetOwnerCharacter();
	// TODO: Understand and FIX this when you will have enough strenght solider
	const FVector Direction = (Victim->VictimDesiredPosition->GetComponentLocation() - Victim->GetActorLocation()) * Victim->GetActorForwardVector().GetSafeNormal();
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Victim->GetActorLocation(), SevenCharacter->GetActorLocation());
	const FVector FinalPosition = Rotation.RotateVector(FVector(Direction.X, 0, 0)) + Victim->GetActorLocation();
	Rotation.Yaw = Rotation.Yaw - 180;
	FTransform T(Rotation, FinalPosition);

	//DrawDebugCoordinateSystem(GetWorld(), T.GetTranslation(), T.GetRotation().Rotator(), 100, true);
	SevenCharacter->AC_MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform("MW_LightAttackAttacker", T);
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
			GetOwnerCharacter()->StopAnimMontage();
		}
		else if (bActiveMontageRunning && !bNextComboTriggerEnabled)
		{
			return false;
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
			GetOwnerCharacter()->StopAnimMontage();
		}
	}

	ASevenCharacter* SevenCharacter = GetOwnerCharacter();
	UAnimInstance* AnimInstance = GetOwnerAnimInstance();

	if (!SevenCharacter || !AnimInstance)
	{
		return false;
	}
	
	//UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] Play Animation %s, section:"), *AnimMontage->GetName(), *SectionName.ToString());

	SevenCharacter->PlayAnimMontage(AnimMontage, 1.0f, SectionName);
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

void UAnimationComponent::OnLayingDead()
{
	USkeletalMeshComponent* PlayerMesh = GetOwnerCharacter()->GetMesh();
	PlayerMesh->SetCollisionProfileName("Ragdoll");
	PlayerMesh->SetSimulatePhysics(true);
	LockedEnemy = nullptr;
	GetOwnerCharacter()->OnLayingDead();
}

void UAnimationComponent::OnEvadeEnded()
{
	GetOwnerCharacter()->bIsEvading = false;
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
		if (bAttackWasPerformed)
		{
			//OnAnimationEnded(nullptr, true);
			
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] Block False"));
			return false;
		}
	}

	ASevenCharacter* const SevenCharacter = GetOwnerCharacter();
	SevenCharacter->bIsBlocking = bEnable;
	SevenCharacter->bIsGuarding = false;
	SevenCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = bEnable;
	SevenCharacter->GetCharacterMovement()->bOrientRotationToMovement = !bEnable;
	SevenCharacter->GetCharacterMovement()->MaxWalkSpeed = bEnable ? 200 : (SevenCharacter->GetIsGuarding() ? 200 : 600);

	return true;
}

bool UAnimationComponent::Guard(bool bEnable)
{
	if (bEnable && IsAnimationRunning())
	{
		return false;
	}
		
	ASevenCharacter* SevenCharacter = GetOwnerCharacter();
	SevenCharacter->bIsBlocking = false;
	SevenCharacter->bIsGuarding = bEnable;
	SevenCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = bEnable;
	SevenCharacter->GetCharacterMovement()->bOrientRotationToMovement = !bEnable;
	SevenCharacter->GetCharacterMovement()->MaxWalkSpeed = bEnable ? 200 : 600;

	if (bEnable)
	{
		const TArray<ASevenCharacter*> FoundEnemies = SevenCharacter->GetEnemiesInFrontOfCharacer(-1, 200, 1000, 100, true);
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
	//UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] OnAnimationEnded Animation: %s, CurrentMontageType: %d, Character: %s, Interrupted: %d"),
	//	*Montage->GetName(), (int)CurrentMontageType, *GetOwnerCharacter()->GetName(), bInterrupted ? 1 :0);
	
	if (!bInterrupted)
	{
		bActiveMontageRunning = false;
		NextMontageType = EMontageType::None;
	}
	else
	{
		CurrentMontageType = NextMontageType;
	}
	
	GetOwnerCharacter()->OnAnimationEnded(CurrentMontageType, NextMontageType);
	GetOwnerCharacter()->AC_AttackComponent->OnAnimationEnded(CurrentMontageType, NextMontageType);
	
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


