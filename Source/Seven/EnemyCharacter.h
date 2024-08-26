#pragma once

#include "CoreMinimal.h"
#include "SevenCharacter.h"
#include "Mission.h"
#include "EnemyCharacter.generated.h"


class UParticleSystem;
class UBehaviorTree;
class UEnemyScenarios;

UCLASS()
class SEVEN_API AEnemyCharacter : public ASevenCharacter
{
	GENERATED_BODY()
	friend class UAICharacter;

public:
	UPROPERTY(BlueprintReadOnly)
	EMissionType MissionType;

	UPROPERTY(BlueprintReadOnly)
	uint8 Difficulty{ 1 };

	UPROPERTY(EditAnywhere, meta = (MustImplement = "ComboInterface", BlueprintBaseOnly))
	TArray<TSubclassOf<UObject>> AvailableCombos;

protected:
	UPROPERTY(EditAnywhere)
	UParticleSystem* EasyAttackParticle;

	UPROPERTY(EditAnywhere)
	UParticleSystem* MidAttackParticle;

	UPROPERTY(EditAnywhere)
	UParticleSystem* HeavyAttackParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UEnemyScenarios* EnemyScenarios;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int LightAttacksAmount{ 1 };

	UPROPERTY(EditAnywhere)
	float AcceptableAttackRadius{ 150.0f };

	virtual ASevenCharacter* FindSevenCharacter() const;

	virtual void Fire(const FInputActionValue& Value) override;

	virtual void AttackEnd() override;

public:
	AEnemyCharacter();
	FORCEINLINE int GetLightAttacksAmount() const { return LightAttacksAmount; };
protected:
	virtual void BeginPlay();

	UFUNCTION(BlueprintCallable)
	virtual void InitiateAttack();
	UFUNCTION(BlueprintCallable)
	void IncomingAttack();
	UFUNCTION(BlueprintCallable)
	void ParryAvailable(bool bEnable);
	void OnSevenCharacterStatusUpdate(const ASevenCharacter* SevenCharacter, const ECharacterState Status);
	virtual void OnAnimationEnded(const EMontageType& StoppedMontage) override;
	virtual void ReceivedHit(const FAttackInfo& AttackInfo) override;
	void SetDefendActionInProgress(const bool bInProgress) const;
	UFUNCTION(BlueprintCallable)
	virtual void MoveTo(bool bToSevenCharacter);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	virtual bool IsNearCharacter() const;
	virtual const FVector GetRandomPointAroundCharacter(const ASevenCharacter* const SevenCharacter);
	UFUNCTION(BlueprintCallable)
	bool TryStealAttackToken();
	virtual void NextAttackAvailable();
	UFUNCTION(BlueprintCallable)
	void DefendActionResolved();
	UFUNCTION(BlueprintCallable)
	void SetLightAttacksAmount(int Amount);
	UFUNCTION(BlueprintCallable)
	UBehaviorTree* GetBehaviorTree() const;
	UFUNCTION(BlueprintCallable)
	void UseCombo(const EComboType ComboType);
	UFUNCTION(BlueprintCallable)
	bool DefendAgainstIncomingAttack(EMontageType DefendMontage);
	UFUNCTION(BlueprintCallable)
	void SetAttackStrength(EAttackStrength NewAttackStrength);
	void SpawnParticles(EAttackStrength NewAttackStrength) const;

	FTimerHandle MovementTimerHandle;

	UFUNCTION(BlueprintCallable)
	void SetMovemenTimertHandle(const FTimerHandle& Handle);
	
	bool HasAttackStarted() const;

	void ReturnAttackToken();
};
