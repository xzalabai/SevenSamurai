#pragma once
#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ESpecial : uint8
{
	ES_None UMETA(DisplayName = "None"),
	ES_Special1 UMETA(DisplayName = "Special 1"),
};

enum class EAttackType
{
	Light = 0,
	Heavy = 1,
};

enum EAttackTypeMontage
{
	Zero = 0,
	One = 1,
	Two = 2,
	Three = 3,
};

struct FAttackInfo
{
	FAttackInfo() = default;
	FAttackInfo(EAttackType AttackType, EAttackTypeMontage AttackTypeMontage, uint8 Damage)
		: AttackType(AttackType), AttackTypeMontage(AttackTypeMontage), Damage(Damage) {}
	EAttackType AttackType = EAttackType::Light;
	EAttackTypeMontage AttackTypeMontage = EAttackTypeMontage::Zero;
	uint8 Damage;

	FString AttackTypeMontageToString() const
	{
		int ATPM_int = static_cast<int>(AttackTypeMontage);
		FString ATPM_FString = FString::FromInt(ATPM_int);

		return ATPM_FString;
	}
};

//struct FAttackInfo
//{
//	FAttackInfo() = default;
//	FAttackInfo(uint8 AttackType, uint8 Damage) : AttackType(AttackType), Damage(Damage) {}
//	uint8 AttackType = 0;
//	uint8 Damage = 0;
//};
