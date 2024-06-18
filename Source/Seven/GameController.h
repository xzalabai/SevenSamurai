#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
//#include "PublicEnums.h"
#include "GameController.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnRestart);

//class ASevenCharacter;

UCLASS()
class SEVEN_API UGameController : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	//UPROPERTY()
	//TMap<int8, EEnemyStatus> EnemiesStatus;
	//
	//UPROPERTY()
	//TMap<int8, EEnemyStatus> SevenCharactersStatus;
	//
	//UPROPERTY()
	//TArray<const ASevenCharacter*> SevenCharacters;
	//
	//UPROPERTY()
	//TArray<const ASevenCharacter*> Enemies;

public:
	FOnRestart OnRestart;

public:
	UFUNCTION()
	void Restart();
};
