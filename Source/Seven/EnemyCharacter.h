#pragma once

#include "CoreMinimal.h"
#include "SevenCharacter.h"
#include "EnemyCharacter.generated.h"


class ASevenPlayerController;
class UParticleSystem;

UCLASS()
class SEVEN_API AEnemyCharacter : public ASevenCharacter
{
	GENERATED_BODY()
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
	
	virtual void ReceivedHit(const FAttackInfo& AttackInfo) override;
	virtual void AttackEnd() const override;
	virtual void OnLayingDead() override;
	UFUNCTION(BlueprintCallable)
	virtual void MoveTo(bool bToSevenCharacter, bool bBlockingStance);
	UFUNCTION(BlueprintCallable)
	bool TryStealAttackToken();
	void ReturnAttackToken();

	UPROPERTY()
	TObjectPtr<ASevenPlayerController> SevenPlayerController;

	UPROPERTY()
	TObjectPtr<ASevenCharacter> SevenCharacterToAttack;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* IncomingAttackParticle;
};
