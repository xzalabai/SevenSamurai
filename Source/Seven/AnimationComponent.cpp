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
	if (SevenCharacter->CanBePossessed() && LockedEnemy)
	{
		FRotator Rot = UKismetMathLibrary::FindLookAtRotation(SevenCharacter->GetActorLocation(), LockedEnemy->GetActorLocation());
		Rot.Pitch = Rot.Pitch - 25.0f;
		CachedPlayerController->SetControlRotation(Rot);
	}
	
	if (IsAttackAnimationRunning() && SevenCharacter->TargetedEnemy)
	{
		WarpAttacker(SevenCharacter->TargetedEnemy);
	}
}

void UAnimationComponent::BeginPlay()
{
	Super::BeginPlay();
	SevenCharacter = Cast<ASevenCharacter>(GetOwner());
	SevenCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	EndDelegate.BindUObject(this, &UAnimationComponent::OnAnimationEnded);
	UAnimInstance* AnimInstance = SevenCharacter->GetMesh()->GetAnimInstance();
	AnimInstance->Montage_SetEndDelegate(EndDelegate);
	
	if (SevenCharacter->CanBePossessed())
	{
		CachedPlayerController = SevenCharacter->GetSevenPlayerController();
	}
}

bool UAnimationComponent::Play(UAnimMontage* AnimMontage, int SectionName, const EMontageType &MontageType)
{
	return Play(AnimMontage, CustomMath::IntToFName(SectionName), MontageType);
}

bool UAnimationComponent::CanPlayAnimation(const EMontageType MontageType) const
{
	int PriorityCurrentMontage = MontagePriorityOrder.Find(CurrentMontage.MontageType);
	int PriorityNextMontage = MontagePriorityOrder.Find(MontageType);

	if (CurrentMontage.MontageType == EMontageType::Parry && (MontageType == EMontageType::LightAttack || MontageType == EMontageType::Combo))
	{
		// Exception: If parry is performed, we want to be able to attack
		return true;
	}

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

void UAnimationComponent::WarpAttacker(const ASevenCharacter* Victim)
{
	// TODO: Understand and FIX this when you will have enough strenght solider
	const FVector Direction = (Victim->VictimDesiredPosition->GetComponentLocation() - Victim->GetActorLocation()) * Victim->GetActorForwardVector().GetSafeNormal();
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Victim->GetActorLocation(), SevenCharacter->GetActorLocation());
	const FVector FinalPosition = Rotation.RotateVector(FVector(Direction.X, 0, 0)) + Victim->GetActorLocation();
	Rotation.Yaw = Rotation.Yaw - 180;
	FTransform T(Rotation, FinalPosition);

	//DrawDebugDirectionalArrow(GetWorld(), FinalPosition, Victim->GetActorLocation(), 12, FColor::Black, true, -1);
	SevenCharacter->AC_MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform("MW_LightAttackAttacker", T);
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
		UE_LOG(LogTemp, Error, TEXT("[UAnimationComponent] AnimInstance is nullptr %s"), *SevenCharacter->GetName());
		return false;
	}

	SevenCharacter->StopAnimMontage();
	SevenCharacter->PlayAnimMontage(AnimMontage, 1.0f, SectionName);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, AnimMontage);
	bActiveMontageRunning = true;
	LastPlayedMontage = CurrentMontage;
	CurrentMontage = FMontage{ MontageType, AnimMontage, Stance == EStances::None ? CurrentMontage.LastStance : Stance };
	SwitchStances(EStances::None, false);
	return true;
}

UAnimInstance* UAnimationComponent::GetOwnerAnimInstance()
{
	if (UAnimInstance* AnimInstance = SevenCharacter->GetMesh()->GetAnimInstance())
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
	USkeletalMeshComponent* PlayerMesh = SevenCharacter->GetMesh();
	PlayerMesh->SetCollisionProfileName("Ragdoll");
	PlayerMesh->SetSimulatePhysics(true);
	LockedEnemy = nullptr;
	SevenCharacter->OnLayingDead();
}

void UAnimationComponent::RotateTowards(const AActor* Actor, const int Shift)
{
	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(SevenCharacter->GetActorLocation(), Actor->GetActorLocation());
	SevenCharacter->RootComponent->SetWorldRotation(PlayerRot);

	if (Shift != 0)
	{
		SevenCharacter->SetActorLocation(SevenCharacter->GetActorLocation() + SevenCharacter->GetActorForwardVector() * Shift);
	}
}

void UAnimationComponent::OnEvadeEnded()
{
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] OnEvadeEnded"));
	CurrentMontage.Reset(false);
}

void UAnimationComponent::NextComboTriggered(bool bEnable)
{
	bNextComboTriggerEnabled = bEnable;
}

bool UAnimationComponent::Block(const bool bEnable, const bool bAffectAnimation)
{
	if (!SevenCharacter)
	{
		// TODO: This should not happen
		return false;
	}

	SevenCharacter->bIsBlocking = bEnable;
	SevenCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = bEnable;
	SevenCharacter->GetCharacterMovement()->bOrientRotationToMovement = !bEnable;
	SevenCharacter->GetCharacterMovement()->MaxWalkSpeed = BlockSpeed;

	if (bAffectAnimation)
	{
		SevenCharacter->bPlayBlockingAnimation = bEnable;
	}

	return true;
}

bool UAnimationComponent::Guard(const bool bEnable, const bool bAffectAnimation)
{
	if (!SevenCharacter)
	{
		// TODO: This should not happen
		return false;
	}
	SevenCharacter->bIsGuarding = bEnable;
	SevenCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = bEnable;
	SevenCharacter->GetCharacterMovement()->bOrientRotationToMovement = !bEnable;
	SevenCharacter->GetCharacterMovement()->MaxWalkSpeed = GuardSpeed;

	if (bAffectAnimation)
	{
		SevenCharacter->bPlayGuardingAnimation = bEnable;
	}

	return true;
}

bool UAnimationComponent::Run(const bool bEnable)
{
	SevenCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = !bEnable;
	SevenCharacter->GetCharacterMovement()->bOrientRotationToMovement = bEnable;
	SevenCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	return true;
}

void UAnimationComponent::LockTarget(bool bEnable, const ASevenCharacter* EnemyToLock)
{
	if (bEnable)
	{
		if (!EnemyToLock)
		{
			const TArray<ASevenCharacter*> FoundEnemies = SevenCharacter->GetEnemiesInFrontOfCharacer(-1, 200, 1000, 100, true);
			if (FoundEnemies.Num() > 0)
			{
				EnemyToLock = FoundEnemies[0];
			}
		}
		LockedEnemy = EnemyToLock;
	}
	else
	{
		LockedEnemy = nullptr;
	}
}

void UAnimationComponent::SwitchStances(const EStances NewStance, const bool bAffectAnimation)
{
	/*
	NewStance: new stance that should be used
	bAffectAnimation: whether we want to turn off the animation (it's desirable to keep animation ON if we receive block hit - so we do not end up in IDLE animation)
	*/

	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent].SwitchStances %s"),
		*UEnum::GetValueAsString(NewStance));

	if (NewStance == Stance)
	{
		return;
	}

	if (Stance == EStances::Block)
	{
		Block(false, bAffectAnimation);
	}
	else if (Stance == EStances::Guard)
	{
		Guard(false, bAffectAnimation);
	}
	else if (Stance == EStances::Run)
	{
		Run(false);
	}

	if (NewStance == EStances::Block)
	{
		Block(true, bAffectAnimation);
	}
	if (NewStance == EStances::Guard)
	{
		Guard(true, bAffectAnimation);
	}
	if (NewStance == EStances::Run)
	{
		Run(true);
	}

	Stance = NewStance;
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
	const FMontage& EndedMontage = bInterrupted ? LastPlayedMontage : CurrentMontage;
	
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] OnAnimationEnded Animation: %s, CurrentMontageType: %s, Character: %s, Interrupted: %d"),
		*Montage->GetName(), *UEnum::GetValueAsString(EndedMontage.MontageType), *SevenCharacter->GetName(), bInterrupted ? 1 :0);

	SevenCharacter->OnAnimationEnded(EndedMontage.MontageType);
	SevenCharacter->AC_AttackComponent->OnAnimationEnded(EndedMontage.MontageType);
	
	if (EndedMontage.MontageType == EMontageType::LightAttack)
	{
		if (!bInterrupted)
		{
			NextComboTriggered(false);
		}
		else
		{
			SevenCharacter->AttackEnd();
		}
	}

	if (!bInterrupted)
	{
		bActiveMontageRunning = false;
		// Set the stance if user made some stance request during montage
		SwitchStances(Stance == EStances::None ? CurrentMontage.LastStance : Stance);
		CurrentMontage.Reset();
	}
	LastPlayedMontage.Reset(); // TODO: Not sure where to put this :(
}

void UAnimationComponent::OnAnimationStarted(UAnimMontage* Montage)
{
	//UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] OnAnimationStarted"));
}


