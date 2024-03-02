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
	class ASevenCharacter* GetCharacterOwner();

	int MaxSections = -1;
	int CurrentSection = -1;

public:	
	UAttackComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	const TPair<UAnimMontage*, FName> GetNextAttackMontage();
	void OnAnimationEnded(const EMontageType& MontageType);

protected:
	virtual void BeginPlay() override;
};
