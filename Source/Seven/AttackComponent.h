#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ComboInterface.h"
#include "PublicEnums.h"
#include "AttackComponent.generated.h"

class UCombo;

UCLASS( ClassGroup=(Custom), BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent) )
class SEVEN_API UAttackComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class ASevenCharacter;

private:
	class UAnimInstance* GetOwnerAnimInstance();
	class ASevenCharacter* GetOwnerCharacter();

	int MaxSections = -1;
	int CurrentSection = -1;

	bool bHeavyAttackReady = false;
	bool bHeavyAttackWasReleased = false;

	EAttackType CurrentAttackType = EAttackType::None;

	UPROPERTY(VisibleAnywhere)
	ESpecial SpecialActivated = ESpecial::ES_None;

	UPROPERTY()
	TMap<int, UObject*> CombosMapping;

	UPROPERTY()
	TObjectPtr<IComboInterface> LastUsedCombo;

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
	void UseCombo(const ESpecial& Special);
	void ComboAttackStart();
	void ComboAttackEnd();

protected:
	virtual void BeginPlay() override;

};
