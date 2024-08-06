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

	int MaxSections = -1;
	int CurrentSection = -1;

	bool bHeavyAttackReady = false;
	bool bHeavyAttackWasReleased = false;

	UPROPERTY(VisibleAnywhere)
	FWeaponDetail WeaponDetail;

	EAttackType CurrentAttackType = EAttackType::None;
	FName CurrentAttackTypeMontage;

	UPROPERTY(VisibleAnywhere)
	ECombo ComboActivated = ECombo::ES_None;

	UPROPERTY()
	TMap<int, UObject*> CombosMapping;

	UPROPERTY()
	TObjectPtr<IComboInterface> LastUsedCombo = nullptr;

public:	
	UAttackComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	const TPair<UAnimMontage*, FName> GetAttackMontageToBePlayed();
	void OnAnimationEnded(const EMontageType& StoppedMontage, const EMontageType& NextMontage);
	FAttackInfo GetAttackInfo() const;
	bool PlayAttack(ASevenCharacter* TargetedEnemy, bool bWarp, bool canInterrupt);
	bool LightAttack(ASevenCharacter* TargetedEnemy);
	void HeavyAttack(ASevenCharacter* TargetedEnemy, const bool bReleased);
	void ThrowKnife();
	UFUNCTION(BlueprintCallable)
	void SetIsHeavyAttackReady(bool bEnable = true);
	bool IsComboAttack();
	void UseCombo(const ECombo& Special);
	void SetCombo(const int8 ID);
	void AddComboToCharacter(TSubclassOf<UObject> TypeOfCombo);
	void ComboAttackStart();
	void ComboAttackEnd();
	TObjectPtr<IComboInterface> GetLastUsedCombo() const;
	int GetWeaponDamage() const;
	bool CanPlayRandomAttackMontage() const;

	UFUNCTION(BlueprintCallable)
	void SetWeaponDamage(const int NewDamage);

	void OnAttackStart();

protected:
	virtual void BeginPlay() override;

};


