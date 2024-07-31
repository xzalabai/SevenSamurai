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

enum class EAttackType
{
	None = 0,
	Light = 1,
	Heavy = 2,
	Combo = 3,
	Throw = 4, // NOT USED ANYMORE, safely DELETE
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
	HeavyAttack,
	Parry,
	LightAttackHitReaction,
	HeavyAttackHitReaction,
	Throw, // NOT USED ANYMORE, safely DELETE
	Block,
	Evade,
	BlockBroken,
	LightAttackHitReactionDeath,
	HeavyAttackHitReactionDeath,
};

static const TArray<EMontageType> AttackMontages{ EMontageType::LightAttack, EMontageType::HeavyAttack };

UENUM(BlueprintType)
enum class EWeaponLevel : uint8
{
	One,
	Two,
	Three,
	Four,
};

UENUM(BlueprintType)
enum class EEnemyStatus : uint8
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
	FAttackInfo(const EAttackType AttackType, const EAttackStrength AllowedHitReaction, const uint8 AttackTypeMontage, const uint8 Damage, AActor* Attacker)
		: AttackType(AttackType),
		AllowedHitReaction(AllowedHitReaction),
		AttackTypeMontage(AttackTypeMontage),
		Damage(Damage),
		Attacker(Attacker)
	{}
	void Reset()
	{
		AttackType = EAttackType::None;
		AllowedHitReaction = EAttackStrength::Light;
		AttackTypeMontage = 0;
		Damage = 0;
		Attacker = nullptr;
	}
	EAttackType AttackType = EAttackType::None;
	EAttackStrength AllowedHitReaction = EAttackStrength::Light;
	uint8 AttackTypeMontage = 0;
	uint8 Damage;
	AActor* Attacker;
	
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