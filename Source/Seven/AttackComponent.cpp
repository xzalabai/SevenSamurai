#include "AttackComponent.h"
#include "AnimationComponent.h"
#include "MotionWarpingComponent.h"
#include "SevenCharacterDA.h"
#include "Combo.h"
#include "SevenCharacter.h"
#include "AnimationsDA.h"
#include "Weapon.h"
#include "ThrowingKnife.h"
#include "EnemyCharacter.h"

UAttackComponent::UAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UAttackComponent::BeginPlay()
{
	Super::BeginPlay();
	// TODO: Doesn't apply for ENEMY!!!!
	if (USevenCharacterDA* SevenCharacterDA = GetOwnerCharacter()->SevenCharacterDA)
	{
		for (TSubclassOf<UObject> UObjectSubclass : SevenCharacterDA->CombosObj)
		{
			AddComboToCharacter(UObjectSubclass);
		}
		WeaponDetail = SevenCharacterDA->WeaponDetail;
	}
	else
	{
		WeaponDetail = FWeaponDetail(200, EWeaponLevel::One);
	}

	CachedSevenCharacter = GetOwnerCharacter();
}


void UAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

const TPair<UAnimMontage*, FName> UAttackComponent::GetAttackMontageToBePlayed()
{
	if (CurrentAttackType != EAttackType::Heavy && CurrentAttackType != EAttackType::Light)
	{
		UE_LOG(LogTemp, Display, TEXT("[UAttackComponent]GetAttackMontageToBePlayed.CurrentAttackType == %d "), (int)CurrentAttackType);
		return TPair<UAnimMontage*, FName>(nullptr, FName());
	}

	UAnimMontage* MontageToBePlayed = CurrentAttackType == EAttackType::Light ? GetOwnerCharacter()->Animations->Montages[EMontageType::LightAttack]
		: CachedSevenCharacter->Animations->Montages[EMontageType::HeavyAttack];

	if (CanPlayRandomAttackMontage())
	{
		// We allow Enemy to play random Attack Montage
		CurrentSection = FMath::RandRange(1, MontageToBePlayed->CompositeSections.Num());
	}
	else
	{
		// Play same montage for Seven
		CurrentSection = 1;
	}

	if (CachedSevenCharacter->AC_Animation->GetCurrentMontageType() == EMontageType::LightAttack)
	{
		// Some Attack animation is in progress
		CurrentSection = CustomMath::FNameToInt(CachedSevenCharacter->AC_Animation->GetCurrentMontageSection());
		if (CurrentSection < MontageToBePlayed->CompositeSections.Num())
		{
			// We can still iterate
			++CurrentSection;
		}
		else
		{
			CurrentSection = 1;
		}

	}
	const FName SectionToPlay = CustomMath::IntToFName(CurrentSection);
	return TPair< UAnimMontage*, FName> (MontageToBePlayed, SectionToPlay);
}


void UAttackComponent::OnAnimationEnded(const EMontageType &StoppedMontage, const EMontageType &NextMontage)
{
	if (StoppedMontage == EMontageType::LightAttack && NextMontage != EMontageType::LightAttack)
	{
		// Reset if Stopped was Attack and it wasn't stopped by another attack
		if (CurrentAttackType == EAttackType::Heavy)
		{
			bHeavyAttackReady = false;
			bHeavyAttackWasReleased = false;
		}
	
		CurrentSection = 1;
		CurrentAttackType = EAttackType::None;
		CurrentAttackTypeMontage = NAME_None;

		LastUsedCombo = nullptr;
	}
	if (StoppedMontage == EMontageType::Throw) // TODO: Can be deleted?
	{
		CurrentAttackType = EAttackType::None;
		CurrentAttackTypeMontage = NAME_None;
	}
}

FAttackInfo UAttackComponent::GetAttackInfo() const
{
	// TODO: Damage based on weapon
	//int DamageToBeDealt = WeaponDetail.Damage;
	int DamageToBeDealt = 20;
	return FAttackInfo(CurrentAttackType, GetOwnerCharacter()->AttackStrength, CustomMath::FNameToInt(CurrentAttackTypeMontage), DamageToBeDealt, GetOwner());
}

bool UAttackComponent::PlayAttack(ASevenCharacter* TargetedEnemy, bool bWarp, bool canInterrupt)
{
	const TPair<UAnimMontage*, FName> NextAttack = GetAttackMontageToBePlayed();
	CurrentAttackTypeMontage = NextAttack.Value;
	
	if (!NextAttack.Key)
	{
		UE_LOG(LogTemp, Error, TEXT("[UAttackComponent]HeavyAttack.NextAttack.Key is nullptr"));
		return false;
	}

	if (GetOwnerCharacter()->AC_Animation->Play(NextAttack.Key, NextAttack.Value, EMontageType::LightAttack, canInterrupt))
	{
		if (TargetedEnemy && bWarp)
		{
			GetOwnerCharacter()->AC_Animation->WarpAttacker("MW_LightAttackAttacker", TargetedEnemy);
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
	// TODO: Find a better place for turning off block!
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(GetOwner());
	SevenCharacter->Block(false);

	if (IsComboAttack())
	{
		UseCombo(ComboActivated);
		CurrentAttackType = EAttackType::Combo;
		CurrentAttackTypeMontage = NAME_None;
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
		CurrentAttackTypeMontage = NAME_None;
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

int UAttackComponent::GetWeaponDamage() const
{
	return WeaponDetail.Damage;
}

bool UAttackComponent::CanPlayRandomAttackMontage() const
{
	if (!CachedSevenCharacter->IsEnemy() || CachedSevenCharacter->AC_Animation->GetCurrentMontageSection() != NAME_None)
	{
		return false;
	}
	
	const AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(CachedSevenCharacter);
	if (Enemy->GetLightAttacksAmount() == 0)
	{
		return true;
	}

	return false;
}

void UAttackComponent::UseCombo(const ECombo& Special)
{
	if (CombosMapping.Num() == 0 || !CombosMapping.Contains((int)Special))
	{
		UE_LOG(LogTemp, Error, TEXT("[UAttackComponent].UseCombo is Empty"));
		ComboAttackEnd();
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("[UAttackComponent].UseCombo %d"), (uint8)Special);
	
	if (IComboInterface* Combo = Cast<IComboInterface>(CombosMapping[(uint8)Special]))
	{
		Combo->Use(GetOwner(), nullptr);
		LastUsedCombo = Combo;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UAttackComponent].UseCombo Combo Cast was not succesfull"));
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
	else if (ID == 2)
	{
		ComboActivated = ECombo::ES_Combo2;
	}
	else if (ID == 3)
	{
		ComboActivated = ECombo::ES_Combo3;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UAttackComponent]SetCombo Unidentified combo."));
	}
}

void UAttackComponent::AddComboToCharacter(TSubclassOf<UObject> TypeOfCombo)
{
	int Index = CombosMapping.Num() + 1;
	CombosMapping.Emplace(Index, NewObject<UObject>(this, TypeOfCombo));
	
	IComboInterface* Combo = Cast<IComboInterface>(CombosMapping[Index]);
	if (Combo && GetOwnerCharacter()->SevenCharacterDA)
	{
		GetOwnerCharacter()->SevenCharacterDA->Combos.Add(Combo->GetComboName());
	}
	
	UE_LOG(LogTemp, Display, TEXT("[UAttackComponent]BuyCombo Combo: %s was added to the Inventory under key %d"), *TypeOfCombo->GetName(), CombosMapping.Num());
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
	ComboActivated = ECombo::ES_None;
	LastUsedCombo = nullptr;
}

void UAttackComponent::SetWeaponDamage(const int NewDamage)
{
	check(NewDamage > 0);
	WeaponDetail.Damage = NewDamage;
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(GetOwner());
	SevenCharacter->SevenCharacterDA->WeaponDetail = FWeaponDetail(WeaponDetail.Damage, WeaponDetail.WeaponLevel);
}

void UAttackComponent::OnAttackStart()
{
	if ((CurrentAttackType == EAttackType::Light || CurrentAttackType == EAttackType::Heavy) && GetOwnerCharacter()->EquippedWeapon)
	{
		GetOwnerCharacter()->EquippedWeapon->AttackStart();
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

const ASevenCharacter* UAttackComponent::GetOwnerCharacter() const
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

