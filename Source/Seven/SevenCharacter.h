#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PublicEnums.h"
#include "EnhancedInputComponent.h"
#include "SevenCharacter.generated.h"

class UAnimMontage;
class AWeapon;
class AThrowingKnife;
class UAICharacter;
class AShield;
class UAnimationComponent;
class UAnimationsDA;
class ASevenPlayerController;
class USevenCharacterDA;
class UAttackComponent;
class UAttributesComponent;
class UMotionWarpingComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEndDelegate); // Serves for Behavior Tree (BTT Attack callbacks)

UENUM(BlueprintType)
enum class EReceivedHitReaction : uint8
{
	NotResolved,
	Blocked,
	Parried,
	Evaded,
	BlockBroken,
	Hit,
	Dead,
};

UCLASS(config = Game)

class ASevenCharacter : public ACharacter
{
	friend class UAnimationComponent;
	friend class UAttributesComponent;
	friend class UAttackComponent;

	friend class UCombo;
	friend class URadialCombo;
	friend class UAICharacter;
	friend class UThrowKnife;
	friend class UShieldDodgeCombo;
	friend class UCallbackCombo;
	friend class UKnifeThrowCombo;
	friend class ACharacterPicker;

	friend class ASevenGameMode;
	friend class UGameController;
	friend class ASevenPlayerController;

	

	GENERATED_BODY()

public:
	UPROPERTY()
	AWeapon* EquippedWeapon;

	UPROPERTY()
	AShield* EquippedShield;

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

	UPROPERTY(EditAnywhere)
	TSubclassOf<AShield> ShieldType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* VictimDesiredPosition;

	UPROPERTY(VisibleAnywhere)
	USevenCharacterDA* SevenCharacterDA;

	UPROPERTY()
	ASevenCharacter* TargetedEnemy{ nullptr };

	UPROPERTY(BlueprintReadOnly)
	EAttackStrength AttackStrength{ EAttackStrength::Light };

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDebugIsImmortal{ false };

	UPROPERTY()
	uint8 uniqueID = 0;

	UPROPERTY()
	uint8 AttackToken = 0;

	UPROPERTY(EditDefaultsOnly)
	UAnimationsDA* Animations;

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnAttackEndDelegate OnAttackEnd;

	//////////////////////////////////////////////////////////////////////////////////////////////////////
	// Components
public:
	UPROPERTY()
	UAnimationComponent* AC_Animation;

	UPROPERTY(EditDefaultsOnly)
	UAttributesComponent* AC_Attribute;

	UPROPERTY()
	UMotionWarpingComponent* AC_MotionWarpingComponent;

	//UPROPERTY(BlueprintReadOnly) // TODO: Why????
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

	UPROPERTY()
	ASevenGameMode* SevenGameMode;

public:
	ASevenCharacter();
	UFUNCTION(BlueprintCallable)
	void PerformWeaponTrace();
	FORCEINLINE bool CanBePossessed() const { return !bEnemy; }
	FORCEINLINE bool IsEnemy() const { return bEnemy; }
	virtual const EAttackStrength GetAttackStrength() const;

protected:
	// Controllable Int
	virtual void Move(const FInputActionValue& Value);
	virtual void Look(const FInputActionValue& Value);
	virtual void Space(const FInputActionValue& Value);
	virtual void Fire(const FInputActionValue& Value);
	virtual void ToggleMovement(const FInputActionValue& Value); // tODO: get rid of this
	virtual void FireRMB(const ETriggerEvent& TriggerEvent);
	UFUNCTION(BlueprintCallable)
	virtual void Block(bool bEnable);
	UFUNCTION(BlueprintCallable)
	virtual void Guard(bool bEnable);
	virtual void StopSpace(const FInputActionValue& Value);
	virtual void Evade(const FInputActionValue& Value);
	virtual void Special(int ID);
	UFUNCTION(BlueprintCallable)
	virtual void AttackStart();
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	UFUNCTION(BlueprintCallable)
	virtual void ComboAttackStart();
	UFUNCTION(BlueprintCallable)
	virtual void ComboAttackEnd();
	UFUNCTION(BlueprintCallable)
	virtual void NextComboTriggered(bool bEnable);
	UFUNCTION(BlueprintCallable)
	virtual void NextAttackAvailable();
	UFUNCTION(BlueprintCallable)
	virtual void AI_MoveToPosition(const FVector& Position);

protected:
	virtual void BeginPlay();
	TArray<ASevenCharacter*> GetEnemiesInFrontOfCharacer(const int8 EnemyID = -1,const int32 StartOffset = 200, const int32 EndOffset = 200, const int32 Thickness = 100, const bool bCameraRelative = true) const;
	ASevenCharacter* GetClosestEnemyInRange(float DotProductTreshold = 0.6);
	void RotateTowards(const AActor* Actor, const int Shift = 0);
	virtual void OnAnimationEnded(const EMontageType& StoppedMontage);
	bool CanParryAttack(const ASevenCharacter* Attacker) const;
	void AttackWasParried();
	EReceivedHitReaction GetHitReaction(const FAttackInfo& AttackInfo);
	void CheckIfBlockingBeforeParrying();
	bool IsEvadingAway(const ASevenCharacter *Enemy) const;
	EOctagonalDirection GetDirection(const FVector2D& Vector) const;
	virtual void OnLayingDead();
	int8 GetMappedComboKey(const EComboType& ComboType) const;

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
	FORCEINLINE bool IsAllowedHitReaction(const EAttackStrength Mask, const EAttackStrength UsedAttackStrength) const { return static_cast<uint8>(Mask) & static_cast<uint8>(UsedAttackStrength); }
	FORCEINLINE bool IsImmortal() const { return bIsImmortal; }
	FORCEINLINE bool GetIsGuarding() const { return bIsGuarding; }
	FORCEINLINE const int GetTargetedEnemyID() const { return (TargetedEnemy && TargetedEnemy->IsAlive()) ? TargetedEnemy->uniqueID : -1; }
	FORCEINLINE bool IsBlockingBeforeAttack() const { return bIsBlockingBeforeAttack; }
	FORCEINLINE uint8 GetUniqueID() const { return uniqueID; }
	UFUNCTION(BlueprintCallable)
	FORCEINLINE ESevenCharacterType GetSevenCharacterType() const { return SevenCharacterType; }
	virtual void ReceivedHit(const FAttackInfo &AttackInfo);
	UFUNCTION(BlueprintCallable)
	void Suicide();
	UAnimMontage* GetVictimMontageToPlay(bool bDeath, const ESevenCharacterType& AttackerCharacterType, const EMontageType& AttackerMontageType, const EComboType& ComboType = EComboType::None) const;
	virtual bool IsAlive() const;
	bool IsSameTeam(const ASevenCharacter* Other) const;
	ASevenPlayerController* GetSevenPlayerController() const;
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool CanStealAttackToken() { return AttackToken == 0; }
	void StealAttackToken(const uint8 enemyUniqueID);
	UFUNCTION(BlueprintCallable)
	void ResetAttackToken();
	FORCEINLINE const uint8 GetAttackTokenOwner() const { return AttackToken; }
};

