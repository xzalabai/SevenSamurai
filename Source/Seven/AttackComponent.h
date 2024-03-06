#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PublicEnums.h"
#include "AttackComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEVEN_API UAttackComponent : public UActorComponent
{
	GENERATED_BODY()


private:
	class UAnimInstance* GetOwnerAnimInstance();
	class ASevenCharacter* GetOwnerCharacter();

	int MaxSections = -1;
	int CurrentSection = -1;

	bool bHeavyAttackReady = false;
	bool bHeavyAttackWasReleased = false;

	EAttackType CurrentAttackType = EAttackType::None;

public:	
	UAttackComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	const TPair<UAnimMontage*, FName> GetAttackMontageToBePlayed();
	void OnAnimationEnded(const EMontageType& StoppedMontage, const EMontageType& NextMontage);
	FAttackInfo GetInfoAboutAttack() const;
	bool PlayAttack(ASevenCharacter* TargetedEnemy, bool bWarp, bool canInterrupt);
	bool LightAttack(ASevenCharacter* TargetedEnemy);
	void HeavyAttack(ASevenCharacter* TargetedEnemy, const bool bReleased);
	UFUNCTION(BlueprintCallable)
	void SetIsHeavyAttackReady(bool bEnable = true);

protected:
	virtual void BeginPlay() override;
};
