#pragma once

#include "CoreMinimal.h"
#include "SevenCharacter.h"
#include "EnemyCharacter.generated.h"


class ASevenPlayerController;
class UParticleSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEndDelegate);

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

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnAttackEndDelegate OnAttackEnd;
	
	virtual void Fire(const FInputActionValue& Value) override;
	virtual void AttackEnd() const override;
	virtual void OnLayingDead() override;
	virtual void TestM();
	UFUNCTION(BlueprintCallable)
	virtual void MoveTo(bool bToSevenCharacter = false);
	virtual const FVector GetRandomGuardPoint();
	UFUNCTION(BlueprintCallable)
	bool TryStealAttackToken();

	UPROPERTY()
	TObjectPtr<ASevenPlayerController> SevenPlayerController;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* IncomingAttackParticle;
};
