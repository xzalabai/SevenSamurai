// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ControllableInterface.h"
#include "InputActionValue.h"
#include "SevenCharacter.generated.h"

class UAnimMontage;
class AWeapon;
class UAnimationComponent;
class UComboManager;

UCLASS(config=Game)

class ASevenCharacter : public ACharacter, public IControllableInterface
{
	friend class UAnimationComponent;

	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponType;

	UPROPERTY()
	AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly)
	bool bEnemy;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Animation Montages
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* LightAttack;
	
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* EvadeMontage;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Components
public:
	UPROPERTY()
	UAnimationComponent* AnimationComponent;

	UPROPERTY(EditDefaultsOnly)
	class ACombo* Combo;
	UPROPERTY(EditDefaultsOnly)
	UComboManager* ComboComponent;

	

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
	virtual void StopSpace(const FInputActionValue& Value) override;
	virtual void Evade(const FInputActionValue& Value) override;
	UFUNCTION(BlueprintCallable)
	virtual void AttackStart() override;
			
protected:
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns AnimationComponent subobject **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UAnimationComponent* GetAnimationComponent() const { return AnimationComponent; }

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	// TEST vars
	ASevenCharacter* EnemyTemporary;
};

