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
class UComboManager;
class AEnemy;
class UMotionWarpingComponent;

UCLASS(config=Game)

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
	bool bIsBlocking;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Animation Montages
private:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* LightAttackAttacker;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* LightAttackVictim;
	
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* EvadeMontage;

	UPROPERTY()
	ASevenCharacter* TargetedEnemy;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Components
public:
	UPROPERTY()
	UAnimationComponent* AnimationComponent;

	UPROPERTY(EditDefaultsOnly)
	UComboManager* ComboComponent;

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
			
protected:
	virtual void BeginPlay();
	TArray<ASevenCharacter*> GetEnemiesInFrontOfCharacer();
	ASevenCharacter* GetClosestEnemyInRange(float DotProductTreshold = 0.6);
	void RotateTowards(const AActor* Actor, const int Shift = 0);
	void OnAnimationEnded();
	EOctagonalDirection GetDirection(const FVector2D& Vector) const;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns AnimationComponent subobject **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UAnimationComponent* GetAnimationComponent() const { return AnimationComponent; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsBlocking() const { return bIsBlocking; }
	virtual void ReceivedHit(const FAttackInfo &AttackInfo);
	
};

