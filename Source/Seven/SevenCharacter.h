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

	UPROPERTY()
	UAnimationComponent* AnimationComponent;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Animation Montages
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* LightAttack;

public:
	ASevenCharacter();
	UFUNCTION(BlueprintCallable)
	void PerformWeaponTrace();

protected:
	// Controllable Int
	virtual void Move(const FInputActionValue& Value) override;
	virtual void Look(const FInputActionValue& Value) override;
	virtual void Space(const FInputActionValue& Value) override;
	virtual void Fire(const FInputActionValue& Value) override;
	virtual void StopSpace(const FInputActionValue& Value) override;
			
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


	// TEST
	FOnMontageEnded EndDelegate;
	void OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted);
};

