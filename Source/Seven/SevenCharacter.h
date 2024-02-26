// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ControllableInterface.h"
#include "PublicEnums.h"
#include "SevenCharacter.generated.h"


class UAnimMontage;
class AWeapon;
class UAnimationComponent;
class UAttributesComponent;
class UComboManager;
class AEnemy;
class UMotionWarpingComponent;

static uint8 UniqueIDCounter = 0;

UCLASS(config = Game)

class ASevenCharacter : public ACharacter, public IControllableInterface
{
	friend class UAnimationComponent;
	friend class UCombo;

	GENERATED_BODY()

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables
//////////////////////////////////////////////////////////////////////////////////////////////////////
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* VictimDesiredPosition;

	UPROPERTY()
	AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly)
	bool bEnemy;

	UPROPERTY(BlueprintReadOnly)
	bool bIsBlocking{ false };

	UPROPERTY(BlueprintReadOnly)
	bool bIsParrying{ false };

	UPROPERTY(BlueprintReadOnly)
	bool bIsImmortal{ false };

	UPROPERTY(BlueprintReadOnly)
	bool bIsEvading{ false };

	UPROPERTY()
	ASevenCharacter* TargetedEnemy;

	UPROPERTY()
	uint8 uniqueID = 0;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Animation Montages
protected:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* LightAttackAttacker;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* LightAttackVictim;
	
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* EvadeMontage;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* BlockMontage;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* ParryMontage;
	//////////////////////////////////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Components
public:
	UPROPERTY()
	UAnimationComponent* AC_Animation;

	UPROPERTY(EditDefaultsOnly)
	UComboManager* ComboComponent;

	UPROPERTY(EditDefaultsOnly)
	UAttributesComponent* AC_Attributes;

	UPROPERTY()
	UMotionWarpingComponent* MotionWarpingComponent;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////////////////////////////////
	
public:
	ASevenCharacter();
	UFUNCTION(BlueprintCallable)
	void PerformWeaponTrace();
	FORCEINLINE bool CanBePossessed() { return !bEnemy; }

protected:
	// Controllable Int
	virtual void Move(const FInputActionValue& Value) override;
	virtual void Look(const FInputActionValue& Value) override;
	virtual void Space(const FInputActionValue& Value) override;
	virtual void Fire(const FInputActionValue& Value) override;
	virtual void Block(bool bEnable) override;
	virtual void StopSpace(const FInputActionValue& Value) override;
	virtual void Evade(const FInputActionValue& Value) override;
	virtual void Special(int ID) override;
	UFUNCTION(BlueprintCallable)
	virtual void AttackStart() override;
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd() override;
			
protected:
	virtual void BeginPlay();
	TArray<ASevenCharacter*> GetEnemiesInFrontOfCharacer(const uint8 EnemyID = -1);
	ASevenCharacter* GetClosestEnemyInRange(float DotProductTreshold = 0.6);
	void RotateTowards(const AActor* Actor, const int Shift = 0);
	void OnAnimationEnded();
	void CheckParrying();
	bool IsEvadingAway(const ASevenCharacter *Enemy);
	EOctagonalDirection GetDirection(const FVector2D& Vector) const;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns AC_Animation subobject **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UAnimationComponent* GetAnimationComponent() const { return AC_Animation; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsBlocking() const { return bIsBlocking; }
	FORCEINLINE bool GetIsImmortal() const { return bIsImmortal; }
	FORCEINLINE bool GetIsEvading() const { return bIsEvading; }
	FORCEINLINE bool GetIsParrying() const { return bIsParrying; }
	FORCEINLINE uint8 GetUniqueID() const { return uniqueID; }
	virtual void ReceivedHit(const FAttackInfo &AttackInfo);
	
};

