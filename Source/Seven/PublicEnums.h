#pragma once
#include "CoreMinimal.h"

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
	// I feel immortal after writing this
	None = 0b000, // Should not be used
	CanBlock = 0b001,
	CanParry = 0b010,
	CanEvade = 0b100,

	Light = 0b111,
	Mid = 0b110,
	Heavy = 0b100,
	Undefendable = 0b000,
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
	Throw, // NOT USED ANYMORE, safely DELETE
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
		EMontageType::Throw,
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
	FAttackInfo(const EMontageType MontageType, const EAttackStrength AllowedHitReaction, const uint8 AttackTypeMontage, const uint8 Damage, AActor* Attacker, EComboType ComboType = EComboType::None)
		: MontageType(MontageType),
		AllowedHitReaction(AllowedHitReaction),
		AttackTypeMontage(AttackTypeMontage),
		Damage(Damage),
		Attacker(Attacker),
		ComboType(ComboType)
	{}
	void Reset()
	{
		MontageType = EMontageType::None;
		AllowedHitReaction = EAttackStrength::Light;
		AttackTypeMontage = 0;
		Damage = 0;
		Attacker = nullptr;
		ComboType = EComboType::None;
	}
	EMontageType MontageType = EMontageType::None;
	EAttackStrength AllowedHitReaction = EAttackStrength::Light;
	uint8 AttackTypeMontage = 0;
	uint8 Damage;
	AActor* Attacker;
	EComboType ComboType{ EComboType::None };
	
};

namespace CustomMath
{
	// No typechecking in these methods

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

	static FName IntToFName(int Number)
	{
		return FName(*FString::FromInt(Number));
	}

	static int FNameToInt(FName Name)
	{
		FString Str = Name.ToString();
		return  FCString::Atoi(*Str);
	}

	static FName ConcatFNameAndInt(const FName& Name, int32 Number)
	{
		FString CombinedString = Name.ToString() + FString::FromInt(Number);
		FName CombinedName(*CombinedString);

		return CombinedName;
	}

	static FName ConcatFNameAndFName(const FName& First, const FName& Second)
	{
		const FString& ConcatenatedString = First.ToString() + Second.ToString();
		return FName(*ConcatenatedString);
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