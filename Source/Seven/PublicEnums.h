#pragma once
#include "CoreMinimal.h"

class ASevenCharacter;

UENUM(BlueprintType)
enum class ECombo : uint8
{
	ES_None UMETA(DisplayName = "None"),
	ES_Combo1 UMETA(DisplayName = "Combo 1"),
	ES_Combo2 UMETA(DisplayName = "Combo 2"),
	ES_Combo3 UMETA(DisplayName = "Combo 3"),
};

UENUM(BlueprintType)
enum class EComboType : uint8
{
	None,
	Throw,
	ClassicRadial,
	ClassicAttackSequence,
	HalbertAttackSequence,
	HalberRadial,
	LancetShieldDodge,
	LancetAttack1,
	LancetAttack2,
};

UENUM(BlueprintType)
enum class EAttackStrength : uint8
{
	None =			0b0000,
	CanBlock =		0b0001,
	CanParry =		0b0010,
	CanEvade =		0b0100,

	Light =			0b1111,
	Mid =			0b1110,
	Heavy =			0b1100,
	Undefendable =	0b1000,
};

UENUM(BlueprintType)
enum class EDayPart : uint8
{
	NotProvided,
	Morning,
	Day,
	Evening,
	Night
};

UENUM(BlueprintType)
enum class ESevenCharacterType : uint8
{
	NotProvided = 0,
	Classic = 1,
	Halbert = 2,
	Lancet = 3,
	Archer = 4,
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Gold UMETA(DisplayName = "Gold"), // KEEP this First!
	HP UMETA(DisplayName = "HP"),
	XP UMETA(DisplayName = "XP"),
	Karma UMETA(DisplayName = "Karma"),
	Rice UMETA(DisplayName = "Rice") // Keep this Last!
};

UENUM(BlueprintType)
enum class EMontageType : uint8
{
	None,
	Misc,
	LightAttack,
	Parry,
	LightAttackHitReaction, // TODO: Remove this (and replace Reactions with something like: Animations->CharacterType->HitReactions->LightAttack / Animations->CharacterType->DeathReactions->LightAttack 
	HitReaction,
	Block,
	Evade,
	BlockBroken,
	Combo,
	LightAttackHitReactionDeath, // just for determing what reaction to play .. same as LightAttackHitReaction
};

static const TArray<EMontageType> AttackMontages{ EMontageType::LightAttack}; // TODO should combo be here?
static const TArray<EMontageType> MontagePriorityOrder
	{
		EMontageType::HitReaction,
		EMontageType::BlockBroken,
		EMontageType::Block,
		EMontageType::Evade,
		EMontageType::Misc,
		EMontageType::Combo,
		EMontageType::LightAttack,
		EMontageType::Parry,
		EMontageType::None,
	};

UENUM(BlueprintType)
enum class EWeaponLevel : uint8
{
	One,
	Two,
	Three,
	Four,
};

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	None,
	Dead,
	AttackEnd,
	IncomingAttack,
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
	FAttackInfo(const EMontageType MontageType, const EAttackStrength AttackStrength, const bool bAttackCanBreakBlock, const uint8 Damage, ASevenCharacter* Attacker, EComboType ComboType = EComboType::None)
		: MontageType(MontageType),
		AttackStrength(AttackStrength),
		bAttackCanBreakBlock(bAttackCanBreakBlock),
		Damage(Damage),
		Attacker(Attacker),
		ComboType(ComboType)
	{}
	void Reset()
	{
		MontageType = EMontageType::None;
		AttackStrength = EAttackStrength::Undefendable;
		bAttackCanBreakBlock = false;
		Damage = 0;
		Attacker = nullptr;
		ComboType = EComboType::None;
	}
	bool IsEmpty()
	{
		if (MontageType == EMontageType::None && !Attacker)
		{
			return true;
		}
		return false;
	}
	
	EMontageType MontageType{ EMontageType::None };
	EAttackStrength AttackStrength{ EAttackStrength::Undefendable};
	bool bAttackCanBreakBlock{ false };
	uint8 Damage{ 0 };
	ASevenCharacter* Attacker{ nullptr };
	EComboType ComboType{ EComboType::None };
	
};

namespace CustomMath
{
	// No typechecking in these methods

	static FString GetRandomNumber_FString(const int Start, const int End)
	{
		return FString::FromInt(FMath::RandRange(Start, End));
	}

	static FName GetRandomNumber_FName(const int Start, const int End)
	{
		return FName(*FString::FromInt(FMath::RandRange(Start, End)));
	}

	static FName IntToFName(const int Number)
	{
		return FName(*FString::FromInt(Number));
	}

	static int FNameToInt(const FName& Name)
	{
		return FCString::Atoi(*Name.ToString());
	}

	static FName ConcatFNameAndInt(const FName& Name, const int32 Number)
	{
		const FString CombinedString = Name.ToString() + FString::FromInt(Number);
		return (*CombinedString);
	}

	static FName ConcatFNameAndFName(const FName& First, const FName& Second)
	{
		const FString ConcatenatedString = First.ToString() + Second.ToString();
		return FName(*ConcatenatedString);
	}
};

namespace OctagonalDirection
{
	static EOctagonalDirection GetOctagonalDirectionInt(int Direction)
	{
		return (EOctagonalDirection)Direction;
	}

	static EOctagonalDirection GetOctagonalDirectionFName(const FName& Direction)
	{
		if (Direction == NAME_None)
		{
			return EOctagonalDirection::None;
		}
		return GetOctagonalDirectionInt(FCString::Atoi(*Direction.ToString()));
	}
}