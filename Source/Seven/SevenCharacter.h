#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ControllableInterface.h"
#include "PublicEnums.h"
#include "SevenCharacter.generated.h"


class UAnimMontage;
class AWeapon;
class AThrowingKnife;
class UAICharacter;
class UAnimationComponent;
class ASevenPlayerController;
class UAttackComponent;
class UAttributesComponent;
class UMotionWarpingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS(config = Game)

class ASevenCharacter : public ACharacter, public IControllableInterface
{
	friend class UAnimationComponent;
	friend class UCombo;
	friend class URadialCombo;
	friend class UAICharacter;
	friend class UAttackComponent;
	friend class UThrowKnife;
	friend class UKnifeThrowCombo;

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
	bool bIsBlockingBeforeAttack{ false };

	UPROPERTY(BlueprintReadOnly)
	bool bIsImmortal{ false };

	UPROPERTY(BlueprintReadOnly)
	bool bIsEvading{ false };

	UPROPERTY(BlueprintReadOnly)
	bool bIsGuarding{ false };

	UPROPERTY()
	uint8 uniqueID = 0;

	UPROPERTY()
	uint8 AttackToken = 0;

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnAttackEndDelegate OnAttackEnd;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Animation Montages
protected:
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* LightAttackAttacker;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* HeavyAttackAttacker;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* HeavyAttackAttackerIdle;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* LightAttackVictim;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* LightAttackVictimDeath;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* HeavyAttackVictim;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* HeavyAttackVictimDeath;
	
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* EvadeMontage;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* BlockBroken;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* BlockMontage;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* ParryMontage;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* ThrowAnimation;
	//////////////////////////////////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Components
public:
	UPROPERTY()
	UAnimationComponent* AC_Animation;

	UPROPERTY(EditDefaultsOnly)
	UAttributesComponent* AC_Attribute;

	UPROPERTY()
	UMotionWarpingComponent* AC_MotionWarpingComponent;

	UPROPERTY(EditAnywhere)
	UAttackComponent* AC_AttackComponent;

	UPROPERTY()
	UAICharacter* AC_AICharacter;

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods
//////////////////////////////////////////////////////////////////////////////////////////////////////


protected:
	UPROPERTY(EditAnywhere)
	TSubclassOf<AThrowingKnife> ThrowingKnifeClass;

	UPROPERTY(EditAnywhere)
	ESevenCharacterType SevenCharacterType;

public:
	ASevenCharacter();
	UFUNCTION(BlueprintCallable)
	void PerformWeaponTrace();
	FORCEINLINE bool CanBePossessed() { return !bEnemy; }
	FORCEINLINE bool IsEnemy() const { return bEnemy; }

protected:
	// Controllable Int
	virtual void Move(const FInputActionValue& Value) override;
	virtual void Look(const FInputActionValue& Value) override;
	virtual void Space(const FInputActionValue& Value) override;
	virtual void Fire(const FInputActionValue& Value) override;
	virtual void ToggleMovement(const FInputActionValue& Value) override;
	virtual void FireRMB(const ETriggerEvent& TriggerEvent) override;
	virtual void Block(bool bEnable) override;
	virtual void StopSpace(const FInputActionValue& Value) override;
	virtual void Evade(const FInputActionValue& Value) override;
	virtual void Special(int ID) override;
	UFUNCTION(BlueprintCallable)
	virtual void AttackStart() override;
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd() const override;
	UFUNCTION(BlueprintCallable)
	virtual void ComboAttackStart() override;
	UFUNCTION(BlueprintCallable)
	virtual void ComboAttackEnd() override;
	UFUNCTION(BlueprintCallable)
	virtual void AI_Fire();
	UFUNCTION(BlueprintCallable)
	virtual void AI_MoveTo(bool bToSevenCharacter, bool bBlockingStance);
	UFUNCTION(BlueprintCallable)
	virtual void AI_MoveToPosition(const FVector& Position);

protected:
	virtual void BeginPlay();
	TArray<ASevenCharacter*> GetEnemiesInFrontOfCharacer(const int8 EnemyID = -1,const int32 StartOffset = 200, const int32 EndOffset = 200, const int32 Thickness = 100, const bool bCameraRelative = true);
	ASevenCharacter* GetClosestEnemyInRange(float DotProductTreshold = 0.6);
	void RotateTowards(const AActor* Actor, const int Shift = 0);
	void OnAnimationEnded(const EMontageType& StoppedMontage, const EMontageType& NextMontage);
	bool ParryAttack(const ASevenCharacter* Attacker);
	void AttackWasParried() const;
	void CheckIfBlockingBeforeParrying();
	bool IsEvadingAway(const ASevenCharacter *Enemy);
	EOctagonalDirection GetDirection(const FVector2D& Vector) const;
	virtual void OnLayingDead();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	/** Returns AC_Animation subobject **/
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UAnimationComponent* GetAnimationComponent() const { return AC_Animation; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UAttackComponent* GetAttackComponent() const { return AC_AttackComponent; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UAICharacter* GetAICharacter() const { return AC_AICharacter; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool GetIsBlocking() const { return bIsBlocking; }
	FORCEINLINE bool GetIsImmortal() const { return bIsImmortal; }
	FORCEINLINE bool GetIsEvading() const { return bIsEvading; }
	FORCEINLINE bool GetIsGuarding() const { return bIsGuarding; }
	FORCEINLINE bool GetIsBlockingBeforeAttack() const { return bIsBlockingBeforeAttack; }
	FORCEINLINE uint8 GetUniqueID() const { return uniqueID; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ESevenCharacterType GetSevenCharacterType() const { return SevenCharacterType; }
	virtual void ReceivedHit(const FAttackInfo &AttackInfo);
	virtual bool IsAlive() const;
	ASevenPlayerController* GetSevenPlayerController() const;
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool CanStealAttackToken() { return AttackToken == 0; }
	void StealAttackToken(const uint8 enemyUniqueID);
	UFUNCTION(BlueprintCallable)
	void ResetAttackToken();
	FORCEINLINE const uint8 GetAttackTokenOwner() const { return AttackToken; }
};

