#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ComboInterface.h"
#include "PublicEnums.h"
#include "Weapon.h"
#include "AttackComponent.generated.h"

class UCombo;
class ASevenCharacter;
class UAnimInstance;
class AThrowingKnife;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SEVEN_API UAttackComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class ASevenCharacter;
	friend class UThrowKnife;
	friend class UKnifeThrowCombo;

private:
	UAnimInstance* GetOwnerAnimInstance();
	ASevenCharacter* GetOwnerCharacter();
	const ASevenCharacter* GetOwnerCharacter() const;

	ASevenCharacter* CachedSevenCharacter{ nullptr };

	UPROPERTY(VisibleAnywhere)
	FWeaponDetail WeaponDetail;

	UPROPERTY(VisibleAnywhere)
	ECombo ComboActivated = ECombo::ES_None;

	UPROPERTY()
	TMap<int, UObject*> CombosMapping;

	UPROPERTY()
	TObjectPtr<IComboInterface> LastUsedCombo = nullptr;

	bool bAttackCanBreakBlock{ false };

public:	
	UAttackComponent();
	const TPair<UAnimMontage*, FName> GetLightAttackMontageToBePlayed();
	void OnAnimationEnded(const EMontageType& StoppedMontage);
	FAttackInfo GetAttackInfo() const;
	EAttackStrength GetAttackStrength() const;
	bool GetAttackCanBreakBlock() const;
	bool LightAttack(ASevenCharacter* TargetedEnemy);
	bool ComboAttack();
	void HeavyAttack(ASevenCharacter* TargetedEnemy, const bool bReleased);
	bool CanUseCombo() const;
	void UseCombo(const ECombo& Special);
	void SetCombo(const int8 ID);
	void AddComboToCharacter(const TSubclassOf<UObject> Combo);
	void ComboAttackStart();
	void ComboAttackEnd();
	TObjectPtr<IComboInterface> GetLastUsedCombo() const;
	int GetWeaponDamage() const;
	bool CanPlayRandomAttackMontage() const;

	UFUNCTION(BlueprintCallable)
	void SetWeaponDamage(const int NewDamage);

protected:
	virtual void BeginPlay() override;

};


