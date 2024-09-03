#include "Combo.h"
#include "SevenCharacter.h"
#include "MotionWarpingComponent.h"
#include "AnimationComponent.h"

void UCombo::Use(AActor* AttackerActor, AActor* VictimActor)
{
	ASevenCharacter* AttackerCharacter = Cast<ASevenCharacter>(AttackerActor);
	ASevenCharacter* VictimCharacter = Cast<ASevenCharacter>(VictimActor);
	if (!AttackerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("ASevenCharacter.Use Attacker Not Found for combo %s"), *GetName());
		return;
	}

	if (!VictimCharacter)
	{
		VictimCharacter = AttackerCharacter->GetClosestEnemyInRange(-1.1f);
		if (!VictimCharacter)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASevenCharacter.Use Victim Not Found for combo %s"), *GetName());
			return;
		}

	}
	// Attacker
	// Rotate and play animation
	AttackerCharacter->GetAnimationComponent()->RotateTowards(VictimCharacter);
	UAnimationComponent* AC_Animation = AttackerCharacter->GetAnimationComponent();
	AC_Animation->Play(AttackerAnimation, "Default", EMontageType::Combo);

	VictimCharacter->ReceivedHit(AttackerCharacter->GetAttackInfo());
	VictimCharacter->AC_MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform("MW_LightAttackComboVictim", AttackerCharacter->VictimDesiredPosition->GetComponentTransform());
}

void UCombo::ComboAttackStart()
{
}

void UCombo::ComboAttackEnd()
{

}

EComboType UCombo::GetComboType() const
{
	return ComboType;
}