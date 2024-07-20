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

protected:
	UPROPERTY()
	TObjectPtr<ASevenCharacter> SevenCharacterToAttack;

	UPROPERTY(EditAnywhere)
	UParticleSystem* IncomingAttackParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UEnemyScenarios* EnemyScenarios;

public:
	AEnemyCharacter();
protected:
	virtual void BeginPlay();

	UFUNCTION(BlueprintCallable)
	virtual void InitiateAttack();
	UFUNCTION(BlueprintCallable)
	void IncomingAttack();
	UFUNCTION(BlueprintCallable)
	void ParryAvailable(bool bEnable);
	
	void OnStatusUpdate(const AActor* Actor, const EEnemyStatus Status);
	virtual void ReceivedHit(const FAttackInfo& AttackInfo) override;
	virtual void AttackEnd() const override;
	virtual void OnLayingDead() override;
	UFUNCTION(BlueprintCallable)
	virtual void MoveTo(bool bToSevenCharacter, bool bBlockingStance);
	UFUNCTION(BlueprintCallable)
	bool TryStealAttackToken();
	UFUNCTION(BlueprintCallable)
	virtual void Block(bool bEnable) override;
	UFUNCTION(BlueprintCallable)
	virtual void PerformEvade();
	UFUNCTION(BlueprintCallable)
	UBehaviorTree* GetBehaviorTree() const;

	void ReturnAttackToken();
};
