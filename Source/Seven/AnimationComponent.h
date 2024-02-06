#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimationComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEVEN_API UAnimationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAnimationComponent();

protected:
	virtual void BeginPlay() override;

public:		
	FOnMontageEnded EndDelegate;
	void OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted);
	void Play(UAnimMontage* AnimMontage, const FName& SectionName);
		
};
