#include "AttackComponent.h"
#include "AnimationComponent.h"
#include "MotionWarpingComponent.h"
#include "Combo.h"
#include "SevenCharacter.h"
#include "Weapon.h"
#include "ThrowingKnife.h"

UAttackComponent::UAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	
	for (int i = 0; i < GetOwnerCharacter()->Combos.Num(); ++i)
	{
		CombosMapping.Emplace(i + 1, NewObject<UObject>(this, GetOwnerCharacter()->Combos[i]));
	}

}


void UAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

//
// NEW APPROACH: Random animation sequence
// 
// const TPair<UAnimMontage*, FName> UAttackComponent::GetAttackMontageToBePlayed()
//{
//	if (CurrentAttackType != EAttackType::Heavy && CurrentAttackType != EAttackType::Light)
//	{
//		UE_LOG(LogTemp, Error, TEXT("[UAttackComponent]GetAttackMontageToBePlayed.CurrentAttackType == %d "), (int)CurrentAttackType);
//		return TPair<UAnimMontage*, FName>(nullptr, FName());
//	}
//
//	const ASevenCharacter* SevenCharacter = GetOwnerCharacter();
//	UAnimMontage* MontageToBePlayed;
//	const FName CurrentlyPlayedSection = SevenCharacter->AC_Animation->GetCurrentMontageSection();
//
//	if (CurrentAttackType == EAttackType::Light)
//	{
//		MontageToBePlayed = GetOwnerCharacter()->LightAttackAttacker;
//		// Random sequence
//		CurrentSection = FMath::RandRange(1, MontageToBePlayed->CompositeSections.Num());
//		if (CurrentlyPlayedSection != NAME_None  && CurrentSection == CustomMath::FNameToInt(CurrentlyPlayedSection))
//		{
//			// We already play this Light Attack animation, try another random try (and if still same, do nothing)...
//			CurrentSection = FMath::RandRange(1, MontageToBePlayed->CompositeSections.Num());
//		}
//		
//	}
//	else if (CurrentAttackType == EAttackType::Heavy)
//	{
//		MontageToBePlayed = GetOwnerCharacter()->HeavyAttackAttacker;
//		// Predetermined sequence
//		if (CurrentlyPlayedSection == NAME_None)
//		{
//			// No Animation in progress
//			CurrentSection = 1;
//		}
//		if (SevenCharacter->AC_Animation->GetCurrentMontageType() == EMontageType::Attack)
//		{
//			// Some Attack animation is in progress
//			CurrentSection = CustomMath::FNameToInt(CurrentlyPlayedSection);
//			if (CurrentSection < MontageToBePlayed->CompositeSections.Num())
//			{
//				// We can still iterate
//				++CurrentSection;
//			}
//			else
//			{
//				CurrentSection = 1;
//				// TODO: HERE SHOULD BE COOLDOWN!
//			}
//		}
//	}
//	const FName SectionToPlay = CustomMath::IntToFName(CurrentSection);
//	return TPair< UAnimMontage*, FName> (MontageToBePlayed, SectionToPlay);
//}

/*
* 
* OLD APPROACH -> Animation is always played in sequence
* 
*/
const TPair<UAnimMontage*, FName> UAttackComponent::GetAttackMontageToBePlayed()
{
	if (CurrentAttackType != EAttackType::Heavy && CurrentAttackType != EAttackType::Light)
	{
		UE_LOG(LogTemp, Error, TEXT("[UAttackComponent]GetAttackMontageToBePlayed.CurrentAttackType == %d "), (int)CurrentAttackType);
		return TPair<UAnimMontage*, FName>(nullptr, FName());
	}

	const ASevenCharacter* SevenCharacter = GetOwnerCharacter();
	UAnimMontage* MontageToBePlayed = CurrentAttackType == EAttackType::Light ? GetOwnerCharacter()->LightAttackAttacker : GetOwnerCharacter()->HeavyAttackAttacker;

	if (SevenCharacter->AC_Animation->GetCurrentMontageSection() == NAME_None)
	{
		// No Animation in progress
		CurrentSection = 1;
	}
	if (SevenCharacter->AC_Animation->GetCurrentMontageType() == EMontageType::Attack)
	{
		// Some Attack animation is in progress
		CurrentSection = CustomMath::FNameToInt(SevenCharacter->AC_Animation->GetCurrentMontageSection());
		if (CurrentSection < MontageToBePlayed->CompositeSections.Num())
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
	return TPair< UAnimMontage*, FName> (MontageToBePlayed, SectionToPlay);
}


void UAttackComponent::OnAnimationEnded(const EMontageType &StoppedMontage, const EMontageType &NextMontage)
{
	if (StoppedMontage == EMontageType::Attack && NextMontage != EMontageType::Attack)
	{
		// Reset if Stopped was Attack and it wasn't stopped by another attack
		if (CurrentAttackType == EAttackType::Heavy)
		{
			bHeavyAttackReady = false;
			bHeavyAttackWasReleased = false;
		}
	
		CurrentSection = 1;
		CurrentAttackType = EAttackType::None;
		LastUsedCombo = nullptr;
	}
	if (StoppedMontage == EMontageType::Throw)
	{
		CurrentAttackType = EAttackType::None;
	}
}

FAttackInfo UAttackComponent::GetInfoAboutAttack() const
{
	// TODO: Damage based on weapon
	return FAttackInfo(CurrentAttackType, 0, CurrentAttackType == EAttackType::Light ? 10 : 20, GetOwner());
}

bool UAttackComponent::PlayAttack(ASevenCharacter* TargetedEnemy, bool bWarp, bool canInterrupt)
{
	const TPair<UAnimMontage*, FName> NextAttack = GetAttackMontageToBePlayed();
	
	if (!NextAttack.Key)
	{
		UE_LOG(LogTemp, Error, TEXT("[UAttackComponent]HeavyAttack.NextAttack.Key is nullptr"));
		return false;
	}

	if (GetOwnerCharacter()->AC_Animation->Play(NextAttack.Key, NextAttack.Value, EMontageType::Attack, canInterrupt))
	{
		if (TargetedEnemy && bWarp)
		{
			GetOwnerCharacter()->AC_Animation->WarpAttacker("MW_LightAttackAttacker", TargetedEnemy);
			UE_LOG(LogTemp, Display, TEXT("[UAttackComponent]LightAttack.TargetedEnemy %s"), *TargetedEnemy->GetName());
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool UAttackComponent::LightAttack(ASevenCharacter* TargetedEnemy)
{
	if (IsComboAttack())
	{
		UseCombo(ComboActivated);
		CurrentAttackType = EAttackType::Combo;
		return true;
	}

	if (CurrentAttackType == EAttackType::Heavy)
	{
		// Don't interrupt heavy
		return false;
	}
	CurrentAttackType = EAttackType::Light;
	return PlayAttack(TargetedEnemy, TargetedEnemy ? true : false, true);
}

void UAttackComponent::HeavyAttack(ASevenCharacter* TargetedEnemy, const bool bReleased)
{
	if (CurrentAttackType != EAttackType::Heavy && bReleased)
	{
		// Was holding mouse and there was not attack
		return;
	}
	else if (CurrentAttackType == EAttackType::None)
	{
		// Initialization phase of attack
		CurrentAttackType = EAttackType::Heavy;
		PlayAttack(nullptr, false, true);
	}
	else if (CurrentAttackType == EAttackType::Heavy && bReleased && !bHeavyAttackReady)
	{
		// Released too soon, wait until Attack is Ready (punishment for Heavy attack)
		bHeavyAttackWasReleased = true;
		return;
	}
	else if (CurrentAttackType == EAttackType::Heavy && bHeavyAttackReady && bReleased)
	{
		// Released during Idle, play attack
		PlayAttack(TargetedEnemy, true, true);
		bHeavyAttackReady = false;
	}
}


void UAttackComponent::StartThrowKnife()
{
	if (GetOwnerCharacter()->AC_Animation->Play(GetOwnerCharacter()->ThrowAnimation, "Default", EMontageType::Throw, false))
	{
		CurrentAttackType = EAttackType::Throw;
	}
}

void UAttackComponent::ThrowKnife()
{
	ASevenCharacter* SevenCharacter = GetOwnerCharacter();

	const TArray<ASevenCharacter*> FoundEnemies = SevenCharacter->GetEnemiesInFrontOfCharacer(-1, 0, 600, 300, true);

	if (FoundEnemies.Num() == 0)
	{
		// Throw in front of you
		return;
	}

	FVector Vector = SevenCharacter->GetMesh()->GetSocketLocation("hand_rSocket");
	Vector = Vector + SevenCharacter->GetActorForwardVector() * 50;
	const FRotator Rotation = FRotator::ZeroRotator;
	// TODO: try to put here CONST throwingKnife, create obj pool
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Owner = GetOwnerCharacter();
	TObjectPtr<AThrowingKnife> ThrowingKnife = GetWorld()->SpawnActor<AThrowingKnife>(SevenCharacter->ThrowingKnifeClass, Vector, Rotation, ActorSpawnParameters);
	ThrowingKnife->FireInDirection(FoundEnemies[0]->GetActorLocation());
}

void UAttackComponent::SetIsHeavyAttackReady(bool bEnable)
{
	if (bHeavyAttackWasReleased)
	{
		// If we already released mouse, perform attack
		PlayAttack(nullptr, false, true);
	}
	bHeavyAttackReady = true;
}

bool UAttackComponent::IsComboAttack()
{
	return ComboActivated != ECombo::ES_None;
}

void UAttackComponent::UseCombo(const ECombo& Special)
{
	UE_LOG(LogTemp, Display, TEXT("[UAttackComponent].UseCombo"));

	if (GetOwnerCharacter()->Combos.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[UAttackComponent].UseCombo is Empty"));
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("[UAttackComponent].UseCombo %d"), (uint8)Special);
	
	if (IComboInterface* Combo = Cast<IComboInterface>(CombosMapping[(uint8)Special]))
	{
		Combo->Use(GetOwner(), nullptr);
		LastUsedCombo = Combo;
	}
	ComboActivated = ECombo::ES_None;
}

void UAttackComponent::SetCombo(const int8 ID)
{
	// Player pressed one of keys for combos (we store it) 
	UE_LOG(LogTemp, Display, TEXT("[UAttackComponent]SetCombo %d is activated."), ID);
	if (ID == 1)
	{
		ComboActivated = ECombo::ES_Combo1;
	}
	if (ID == 2)
	{
		ComboActivated = ECombo::ES_Combo2;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UAttackComponent]SetCombo Unidentified combo."));
	}
}

void UAttackComponent::ComboAttackStart()
{
	if (LastUsedCombo)
	{
		LastUsedCombo->ComboAttackStart();
	}
}

void UAttackComponent::ComboAttackEnd()
{
	if (LastUsedCombo)
	{
		LastUsedCombo->ComboAttackEnd();
	}

	LastUsedCombo = nullptr;
}

void UAttackComponent::OnAttackStart()
{
	if ((CurrentAttackType == EAttackType::Light || CurrentAttackType == EAttackType::Heavy) && GetOwnerCharacter()->EquippedWeapon)
	{
		GetOwnerCharacter()->EquippedWeapon->AttackStart();
	}
	else if (CurrentAttackType == EAttackType::Throw)
	{
		ThrowKnife();
	}
}

ASevenCharacter* UAttackComponent::GetOwnerCharacter()
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

