#include "Combo.h"
#include "SevenCharacter.h"
#include "AnimationComponent.h"

ACombo::ACombo()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACombo::Use(AActor* Attacker, AActor* Victim)
{
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(Attacker);
	if (!SevenCharacter)
	{
		return;
	}
	UAnimationComponent* AnimationComponent = SevenCharacter->GetAnimationComponent();
	AnimationComponent->Play(AttackerAnimation, "Default", false);

}


