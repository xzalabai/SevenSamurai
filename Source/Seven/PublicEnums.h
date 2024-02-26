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

UENUM(BlueprintType)
enum class EEnemyStatus : uint8
{
	None,
	Dead,
	Cooldown,
	IncomingAttack,
	Attack,	
	ParryAvailable,
	ParryUnavailable,
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
	FAttackInfo(EAttackType AttackType, uint8 AttackTypeMontage, uint8 Damage, AActor* Attacker)
		: AttackType(AttackType), AttackTypeMontage(AttackTypeMontage), Damage(Damage), Attacker(Attacker) {}
	EAttackType AttackType = EAttackType::Light;
	uint8 AttackTypeMontage = 0;
	uint8 Damage;
	const AActor* const Attacker;
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

namespace OctagonalDirection
{
	static EOctagonalDirection GetOctagonalDirectionInt(int Direction)
	{
		return (EOctagonalDirection)Direction;
	}

	static EOctagonalDirection GetOctagonalDirectionFName(FName Direction)
	{
		if (Direction == NAME_None)
			return EOctagonalDirection::None;

		FString Text = Direction.ToString();
		int Num = FCString::Atoi(*Text);
		return GetOctagonalDirectionInt(Num);
	}
}