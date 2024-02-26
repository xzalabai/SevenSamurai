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
	UFUNCTION(BlueprintCallable)
	void InitiateAttack();

	UFUNCTION(BlueprintCallable)
	void IncomingAttack();

	UFUNCTION(BlueprintCallable)
	void ParryAvailable(bool bEnable);

	virtual void Fire(const FInputActionValue& Value) override;
	virtual void AttackEnd() override;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnAttackEndDelegate OnAttackEnd;


private:
	UPROPERTY()
	TObjectPtr<ASevenPlayerController> SevenPlayerController;
	
};
