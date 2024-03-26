#pragma once

#include "CoreMinimal.h"
#include "SevenCharacter.h"
#include "EnemyCharacter.generated.h"


class ASevenPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class SEVEN_API AEnemyCharacter : public ASevenCharacter
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay();

	UFUNCTION(BlueprintCallable)
	void InitiateAttack();

	UFUNCTION(BlueprintCallable)
	void IncomingAttack();

	UFUNCTION(BlueprintCallable)
	void ParryAvailable(bool bEnable);

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnAttackEndDelegate OnAttackEnd;
	
	virtual void Fire(const FInputActionValue& Value) override;
	virtual void AttackEnd() const override;
	virtual void OnLayingDead() override;

private:
	UPROPERTY()
	TObjectPtr<ASevenPlayerController> SevenPlayerController;
	
};
