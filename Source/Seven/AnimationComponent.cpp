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
		// TODO remove creating FSTRING EVERY TICK Omg!
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

bool UAnimationComponent::Play(UAnimMontage* AnimMontage, int SectionName, const EMontageType &MontageType)
{
	FString myString = FString::FromInt(SectionName);
	return Play(AnimMontage, FName(*myString), MontageType);
}

bool UAnimationComponent::CanPlayAnimation(const EMontageType MontageType) const
{
	int PriorityCurrentMontage = MontagePriorityOrder.Find(CurrentMontage.MontageType);
	int PriorityNextMontage = MontagePriorityOrder.Find(MontageType);

	if (PriorityCurrentMontage < PriorityNextMontage)
	{
		return false;
	}
	if (PriorityCurrentMontage > PriorityNextMontage)
	{
		return true;
	}
	if (PriorityCurrentMontage == PriorityNextMontage)
	{
		if (MontageType == EMontageType::LightAttack && bNextComboTriggerEnabled)
		{
			return true;
		}
		if (MontageType == EMontageType::HitReaction)
		{
			return true;
		}
		return false;
	}

	return false;
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

bool UAnimationComponent::Play(UAnimMontage* AnimMontage, const FName& SectionName, const EMontageType MontageType)
{
	if (!CanPlayAnimation(MontageType))
	{
		return false;
	}

	UAnimInstance* AnimInstance = GetOwnerAnimInstance();

	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("[UAnimationComponent] AnimInstance is nullptr %s"), *CachedSevenCharacter->GetName());
		return false;
	}

	CachedSevenCharacter->StopAnimMontage();
	CachedSevenCharacter->PlayAnimMontage(AnimMontage, 1.0f, SectionName);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AnimMontage);
	bActiveMontageRunning = true;
	LastPlayedMontage = CurrentMontage;
	CurrentMontage = FMontage{ MontageType, AnimMontage };
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
	
	if (!CachedSevenCharacter /*TODO this is just for the bug where Enemy hits on the first frame! :D */ && bEnable && IsAnimationRunning())
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
		return AnimInstance->Montage_GetCurrentSection(CurrentMontage.AnimMontage);
	}
	return NAME_None;
}

void UAnimationComponent::OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted)
{
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] OnAnimationEnded Animation: %s, CurrentMontageType: %d, Character: %s, Interrupted: %d"),
		*Montage->GetName(), (int)CurrentMontage.MontageType, *CachedSevenCharacter->GetName(), bInterrupted ? 1 :0);

	CachedSevenCharacter->OnAnimationEnded(LastPlayedMontage.MontageType);
	CachedSevenCharacter->AC_AttackComponent->OnAnimationEnded(LastPlayedMontage.MontageType);
	
	if (LastPlayedMontage.MontageType == EMontageType::LightAttack)
	{
		if (!bInterrupted)
		{
			NextComboTriggered(false);
		}
		else
		{
			CachedSevenCharacter->AttackEnd();
		}
	}

	if (!bInterrupted)
	{
		bActiveMontageRunning = false;
		CurrentMontage.Reset();
		
	}
	LastPlayedMontage.Reset(); // TODO: Not sure where to put this :(
}

void UAnimationComponent::OnAnimationStarted(UAnimMontage* Montage)
{
	//UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] OnAnimationStarted"));
}


