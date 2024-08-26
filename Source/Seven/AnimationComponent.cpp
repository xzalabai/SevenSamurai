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
	if (CachedSevenCharacter->CanBePossessed() && LockedEnemy)
	{
		FRotator Rot = UKismetMathLibrary::FindLookAtRotation(CachedSevenCharacter->GetActorLocation(), LockedEnemy->GetActorLocation());
		Rot.Pitch = Rot.Pitch - 25.0f;
		CachedPlayerController->SetControlRotation(Rot);
	}
	
	if (IsAttackAnimationRunning() && CachedSevenCharacter->TargetedEnemy)
	{
		WarpAttacker(CachedSevenCharacter->TargetedEnemy);
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
	UE_LOG(LogTemp, Error, TEXT("[UAnimationComponent] BeginPlay for %s"), *GetName());
	
	if (CachedSevenCharacter->CanBePossessed())
	{
		CachedPlayerController = CachedSevenCharacter->GetSevenPlayerController();
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
	CurrentMontage = FMontage{ MontageType, AnimMontage, Stance };
	SwitchStances(EStances::None);
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

void UAnimationComponent::RotateTowards(const AActor* Actor, const int Shift)
{
	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(CachedSevenCharacter->GetActorLocation(), Actor->GetActorLocation());
	CachedSevenCharacter->RootComponent->SetWorldRotation(PlayerRot);

	if (Shift != 0)
	{
		CachedSevenCharacter->SetActorLocation(CachedSevenCharacter->GetActorLocation() + CachedSevenCharacter->GetActorForwardVector() * Shift);
	}
}

void UAnimationComponent::OnEvadeEnded()
{
	UE_LOG(LogTemp, Display, TEXT("[UAnimationComponent] OnEvadeEnded"));
	CurrentMontage.Reset();
}

void UAnimationComponent::NextComboTriggered(bool bEnable)
{
	bNextComboTriggerEnabled = bEnable;
}

bool UAnimationComponent::Block(const bool bEnable)
{
	if (!CachedSevenCharacter)
	{
		// TODO: This should not happen
		return false;
	}

	bool &bAttackWasPerformed = bNextComboTriggerEnabled; // We use bNextComboTriggerEnabled also as an indicator of whether attack was already performed.

	CachedSevenCharacter->bIsBlocking = bEnable;
	CachedSevenCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = bEnable;
	CachedSevenCharacter->GetCharacterMovement()->bOrientRotationToMovement = !bEnable;
	CachedSevenCharacter->GetCharacterMovement()->MaxWalkSpeed = BlockSpeed;

	return true;
}

bool UAnimationComponent::Guard(const bool bEnable)
{
	if (!CachedSevenCharacter)
	{
		// TODO: This should not happen
		return false;
	}
	CachedSevenCharacter->bIsGuarding = bEnable;
	CachedSevenCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = bEnable;
	CachedSevenCharacter->GetCharacterMovement()->bOrientRotationToMovement = !bEnable;
	CachedSevenCharacter->GetCharacterMovement()->MaxWalkSpeed = GuardSpeed;
	return true;
}

bool UAnimationComponent::Run(const bool bEnable)
{
	CachedSevenCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = !bEnable;
	CachedSevenCharacter->GetCharacterMovement()->bOrientRotationToMovement = bEnable;
	CachedSevenCharacter->GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	return true;
}

void UAnimationComponent::LockTarget(bool bEnable, const ASevenCharacter* EnemyToLock)
{
	if (bEnable)
	{
		if (!EnemyToLock)
		{
			const TArray<ASevenCharacter*> FoundEnemies = CachedSevenCharacter->GetEnemiesInFrontOfCharacer(-1, 200, 1000, 100, true);
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

void UAnimationComponent::SwitchStances(const EStances NewStance)
{
	if (NewStance == Stance)
	{
		return;
	}

	if (Stance == EStances::Block)
	{
		Block(false);
	}
	else if (Stance == EStances::Guard)
	{
		Guard(false);
	}
	else if (Stance == EStances::Run)
	{
		Run(false);
	}

	if (NewStance == EStances::Block)
	{
		Block(true);
	}
	if (NewStance == EStances::Guard)
	{
		Guard(true);
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
		*Montage->GetName(), *UEnum::GetValueAsString(EndedMontage.MontageType), *CachedSevenCharacter->GetName(), bInterrupted ? 1 :0);

	CachedSevenCharacter->OnAnimationEnded(EndedMontage.MontageType);
	CachedSevenCharacter->AC_AttackComponent->OnAnimationEnded(EndedMontage.MontageType);
	
	if (EndedMontage.MontageType == EMontageType::LightAttack)
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


