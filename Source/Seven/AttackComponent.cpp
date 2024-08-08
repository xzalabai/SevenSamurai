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

const TPair<UAnimMontage*, FName> UAttackComponent::GetLightAttackMontageToBePlayed()
{
	UAnimMontage* MontageToBePlayed = GetOwnerCharacter()->Animations->Montages[EMontageType::LightAttack];
	int CurrentSection = 1;
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
		// Attack animation is in progress
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
	return TPair<UAnimMontage*, FName> (MontageToBePlayed, SectionToPlay);
}


void UAttackComponent::OnAnimationEnded(const EMontageType &StoppedMontage)
{
	LastUsedCombo = nullptr; // TODO Maybe bad
}

FAttackInfo UAttackComponent::GetAttackInfo() const
{
	// TODO: Damage based on weapon
	//int DamageToBeDealt = WeaponDetail.Damage;
	int DamageToBeDealt = 20;
	EComboType ComboType = LastUsedCombo ? LastUsedCombo->GetComboType() : EComboType::None;
	EMontageType AttackerMontageType = CachedSevenCharacter->AC_Animation->CurrentMontage.MontageType;
	int AttackerMontageSection = CustomMath::FNameToInt(CachedSevenCharacter->AC_Animation->GetCurrentMontageSection());
	UE_LOG(LogTemp, Warning, TEXT("ATTACK TYPE MONTAGE: %d"), (int)AttackerMontageSection);

	return FAttackInfo(AttackerMontageType, GetOwnerCharacter()->AttackStrength, AttackerMontageSection, DamageToBeDealt, CachedSevenCharacter, ComboType);
}

bool UAttackComponent::LightAttack(ASevenCharacter* TargetedEnemy)
{
	// TODO: Find a better place for turning off block!
	CachedSevenCharacter->Block(false);
	const TPair<UAnimMontage*, FName> NextAttack = GetLightAttackMontageToBePlayed();
	bool bAnimationIsPlaying = CachedSevenCharacter->AC_Animation->Play(NextAttack.Key, NextAttack.Value, EMontageType::LightAttack);
	return bAnimationIsPlaying;
}

bool UAttackComponent::ComboAttack()
{
	if (CanUseCombo())
	{
		UseCombo(ComboActivated); // TODO can combo interrupt everything???
		return true;
	}
	return false;
}

void UAttackComponent::HeavyAttack(ASevenCharacter* TargetedEnemy, const bool bReleased)
{
		
}

bool UAttackComponent::CanUseCombo() const
{
	if (CombosMapping.Num() == 0 || !CombosMapping.Contains((int)ComboActivated))
	{
		return false;
	}

	if (ComboActivated != ECombo::ES_None && CachedSevenCharacter->AC_Animation->CanPlayAnimation(EMontageType::Combo))
	{
		return true;
	}
	return false;
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
	UE_LOG(LogTemp, Display, TEXT("[UAttackComponent].UseCombo %d"), (uint8)Special);
	
	if (IComboInterface* Combo = Cast<IComboInterface>(CombosMapping[(uint8)Special]))
	{
		Combo->Use(GetOwner(), CachedSevenCharacter->TargetedEnemy);
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

TObjectPtr<IComboInterface> UAttackComponent::GetLastUsedCombo() const
{
	return LastUsedCombo;
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
	GetOwnerCharacter()->EquippedWeapon->AttackStart();
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

