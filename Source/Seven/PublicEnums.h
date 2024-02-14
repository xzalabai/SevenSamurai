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

struct FAttackInfo
{
	FAttackInfo() = default;
	FAttackInfo(EAttackType AttackType, uint8 AttackTypeMontage, uint8 Damage)
		: AttackType(AttackType), AttackTypeMontage(AttackTypeMontage), Damage(Damage) {}
	EAttackType AttackType = EAttackType::Light;
	uint8 AttackTypeMontage = 0;
	uint8 Damage;
};