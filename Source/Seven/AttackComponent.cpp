#include "AttackComponent.h"
#include "AnimationComponent.h"
#include "AttributesComponent.h"
#include "MotionWarpingComponent.h"
#include "SevenCharacterDA.h"
#include "Combo.h"
#include "SevenCharacter.h"
#include "EnemyLevelDA.h"
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
		check(SevenCharacterDA->CombosObj.Num() < 3);
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
	CombosMapping.Reserve(5);
	CachedSevenCharacter = GetOwnerCharacter();
	AC_Animation = CachedSevenCharacter->AC_Animation;
	AC_Attribute = CachedSevenCharacter->AC_Attribute;
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

	if (AC_Animation->GetCurrentMontageType() == EMontageType::LightAttack)
	{
		// Attack animation is in progress
		CurrentSection = CustomMath::FNameToInt(AC_Animation->GetCurrentMontageSection());
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
	if (StoppedMontage == EMontageType::Combo)
	{
		LastUsedCombo = nullptr;
	}
}

FAttackInfo UAttackComponent::GetAttackInfo() const
{
	const int DamageToBeDealt = GetDamage();
	const EMontageType AttackerMontageType = AC_Animation->CurrentMontage.MontageType;
	const EAttackStrength AttackStrength = GetAttackStrength();
	const EComboType ComboType = LastUsedCombo ? LastUsedCombo->GetComboType() : EComboType::None;
	UE_LOG(LogTemp, Display, TEXT("[UAttackComponent].GetAttackInfo:\nAttackerMontageType: %s \n, AttackStrength: %s \n, ComboType: %s \n DamageToBeDealth %d\n"),
		*UEnum::GetValueAsString(AttackerMontageType),
		*UEnum::GetValueAsString(AttackStrength),
		*UEnum::GetValueAsString(ComboType),
		DamageToBeDealt
		);

	return FAttackInfo(AttackerMontageType, AttackStrength, DamageToBeDealt, CachedSevenCharacter, ComboType);
}

EAttackStrength UAttackComponent::GetAttackStrength() const
{
	const EMontageType CurrentMontageType = AC_Animation->CurrentMontage.MontageType;
	const EComboType ComboType = LastUsedCombo ? LastUsedCombo->GetComboType() : EComboType::None;

	switch (CurrentMontageType)
	{
		case EMontageType::LightAttack:
			return CachedSevenCharacter->AttackStrength;
		case EMontageType::Combo:
			if (!CachedSevenCharacter->IsEnemy())
			{
				return EAttackStrength::Undefendable;
			}
			switch (ComboType)
			{
				case EComboType::Throw:
					return EAttackStrength::Light;
				case EComboType::LancetAttack2:
					return EAttackStrength::Light;
				case EComboType::None:
					UE_LOG(LogTemp, Error, TEXT("[UAttackComponent]GetDamage LastUsedCombo is nullptr"), (int)CurrentMontageType);
					return EAttackStrength::Light;
				default:
					return EAttackStrength::Heavy;
			}
		default:
			UE_LOG(LogTemp, Error, TEXT("[UAttackComponent]GetDamage CurrentMontageType != [LightAttack, CurrentMontageType], MontageType: %d"), (int)CurrentMontageType);
			return EAttackStrength::Light;
	}
}

uint8 UAttackComponent::GetDamage() const
{
	const EMontageType CurrentMontageType = AC_Animation->CurrentMontage.MontageType;
	const EComboType ComboType = LastUsedCombo ? LastUsedCombo->GetComboType() : EComboType::None;

	switch (CurrentMontageType)
	{
		case EMontageType::LightAttack:
			return 1;
			break;
		case EMontageType::Combo:
			switch (ComboType)
			{
				case EComboType::None:
					UE_LOG(LogTemp, Error, TEXT("[UAttackComponent]GetDamage LastUsedCombo is nullptr"));
					return 1;
				case EComboType::Throw:
					return 1;
				default:
					return 2;
			}
			return 1;
		default:
			UE_LOG(LogTemp, Error, TEXT("[UAttackComponent]GetDamage CurrentMontageType != [LightAttack, CurrentMontageType], MontageType: %d"), (int)CurrentMontageType);
			return 1;
	}
}

bool UAttackComponent::LightAttack(ASevenCharacter* TargetedEnemy)
{
	// TODO: Find a better place for turning off block!
	AC_Animation->SwitchStances(EStances::Guard);
	const TPair<UAnimMontage*, FName> NextAttack = GetLightAttackMontageToBePlayed();
	bool bAnimationIsPlaying = AC_Animation->Play(NextAttack.Key, NextAttack.Value, EMontageType::LightAttack);
	return bAnimationIsPlaying;
}

bool UAttackComponent::ComboAttack()
{
	if (CanUseCombo())
	{
		UseCombo(ComboActivated);
		AC_Attribute->Decrease(EItemType::XP, 1);
		return true;
	}
	ComboActivated = ECombo::ES_None;
	return false;
}

void UAttackComponent::HeavyAttack(ASevenCharacter* TargetedEnemy, const bool bReleased)
{
		
}

bool UAttackComponent::CanUseCombo() const
{
	if (AC_Attribute->GetXP() < 1)
	{
		return false;
	}

	if (CombosMapping.Num() == 0 || !CombosMapping.Contains((int)ComboActivated))
	{
		return false;
	}

	if (ComboActivated != ECombo::ES_None && AC_Animation->CanPlayAnimation(EMontageType::Combo))
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
	if (!CachedSevenCharacter->IsEnemy() || AC_Animation->GetCurrentMontageSection() != NAME_None)
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
	IComboInterface* Combo = Cast<IComboInterface>(CombosMapping[(uint8)Special]);
	LastUsedCombo = Combo;
	Combo->Use(CachedSevenCharacter, CachedSevenCharacter->TargetedEnemy);
	ComboActivated = ECombo::ES_None;
}

void UAttackComponent::SetCombo(const int8 ID)
{
	UE_LOG(LogTemp, Display, TEXT("[UAttackComponent]SetCombo %d is activated."), ID);
	switch (ID)
	{
	case 1:
		ComboActivated = ECombo::ES_Combo1;
		break;
	case 2:
		ComboActivated = ECombo::ES_Combo2;
		break;
	case 3:
		ComboActivated = ECombo::ES_Combo3;
		break;
	default:
		ComboActivated = ECombo::ES_None;
		UE_LOG(LogTemp, Error, TEXT("[UAttackComponent]SetCombo Unidentified combo."));
	}
}

void UAttackComponent::AddComboToCharacter(const TSubclassOf<UObject> Combo)
{
	CombosMapping.Emplace(CombosMapping.Num() + 1, NewObject<UObject>(this, Combo));

	const IComboInterface* ICombo = Cast<IComboInterface>(CombosMapping[CombosMapping.Num()]);
	if (!ICombo)
	{
		UE_LOG(LogTemp, Fatal, TEXT("[UAttackComponent]AddComboToCharacter Combo has no EComboType Defined %s"), *Combo.GetDefaultObject()->GetName());
	}
	if (ICombo->GetComboType() == EComboType::None)
	{
		UE_LOG(LogTemp, Fatal, TEXT("[UAttackComponent]AddComboToCharacter Combo's EComboType is unknown type: %d"), (int)ICombo->GetComboType());
	}
	
	UE_LOG(LogTemp, Display, TEXT("[UAttackComponent]AddComboToCharacter Combo of type EComboType: %d was added under %d"), ICombo->GetComboType(), CombosMapping.Num());
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

