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

enum class EOctagonalDirection
{
	Forward = 0,
	ForwardLeft = 1,
	ForwardRight = 2,
	
	Backward = 3,
	BackwardLeft = 4,
	BackwardRight = 5,
	
	Left = 6,
	Right = 7,
	None = 8,
};

struct FAttackInfo
{
	FAttackInfo() = default;
	FAttackInfo(EAttackType AttackType, uint8 AttackTypeMontage, uint8 Damage, uint8 AttackerID)
		: AttackType(AttackType), AttackTypeMontage(AttackTypeMontage), Damage(Damage), AttackerID(AttackerID) {}
	EAttackType AttackType = EAttackType::Light;
	uint8 AttackTypeMontage = 0;
	uint8 Damage;
	uint8 AttackerID;
};

namespace CustomMath
{

	static FString GetRandomNumber_FString(int Start, int End)
	{
		int RandomMontage = FMath::RandRange(Start, End);
		return FString::FromInt(RandomMontage);
	}

	static FName GetRandomNumber_FName(int Start, int End)
	{
		int RandomMontage = FMath::RandRange(Start, End);
		return FName(*FString::FromInt(RandomMontage));
	}
};