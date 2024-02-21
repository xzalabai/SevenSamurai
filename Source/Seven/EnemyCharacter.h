#pragma once

#include "CoreMinimal.h"
#include "SevenCharacter.h"
#include "EnemyCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class SEVEN_API AEnemyCharacter : public ASevenCharacter
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintCallable)
	void InitiateAttack();

	virtual void Fire(const FInputActionValue& Value) override;
	virtual void AttackEnd() override;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnAttackEndDelegate OnAttackEnd;

	
};
