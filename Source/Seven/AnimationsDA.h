#pragma once

#include "CoreMinimal.h"
#include "PublicEnums.h"
#include "Engine/DataAsset.h"
#include "AnimationsDA.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FAnimationReaction
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	TMap<EMontageType, UAnimMontage*> AnimationMapping;
};

UCLASS()
class SEVEN_API UAnimationsDA : public UDataAsset
{
	GENERATED_BODY() 
public:	
	UPROPERTY(EditDefaultsOnly)
	ESevenCharacterType SevenCharacterType;

	UPROPERTY(EditDefaultsOnly)
	TMap<EMontageType, UAnimMontage*> Montages;

	UPROPERTY(EditDefaultsOnly)
	TMap<ESevenCharacterType, FAnimationReaction> Reactions;
};
