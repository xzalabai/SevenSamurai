#include "KnifeThrowCombo.h"
#include "ThrowingKnife.h"
#include "AnimationComponent.h"
#include "AttackComponent.h"
#include "SevenCharacter.h"

void UKnifeThrowCombo::Use(AActor* AttackerActor, AActor* VictimActor)
{
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(AttackerActor);
	if (SevenCharacter->AC_Animation->Play(ThrowAnimation, "Default", EMontageType::LightAttack, false))
	{
		SevenCharacter->AC_AttackComponent->CurrentAttackType = EAttackType::Combo;
		Attacker = SevenCharacter;
	}
}

void UKnifeThrowCombo::ComboAttackStart()
{
	if (!Attacker)
	{
		UE_LOG(LogTemp, Error, TEXT("[UKnifeThrowCombo] ComboAttackStart"));
		return;
	}

	const TArray<ASevenCharacter*> FoundEnemies = Attacker->GetEnemiesInFrontOfCharacer(-1, 0, 1600, 300, true);

	if (FoundEnemies.Num() == 0)
	{
		// Throw in front of you
		return;
	}

	FVector Vector = Attacker->GetMesh()->GetSocketLocation("hand_rSocket");
	Vector = Vector + Attacker->GetActorForwardVector() * 50;
	const FRotator Rotation = FRotator::ZeroRotator;
	// TODO: try to put here CONST throwingKnife, create obj pool
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.Owner = Attacker;
	TObjectPtr<AThrowingKnife> ThrowingKnife = GetWorld()->SpawnActor<AThrowingKnife>(ThrowingKnifeClass, Vector, Rotation, ActorSpawnParameters);
	ThrowingKnife->FireInDirection(FoundEnemies[0]->GetActorLocation());
}

void UKnifeThrowCombo::ComboAttackEnd()
{

}

const FName& UKnifeThrowCombo::GetComboName()
{
	return ComboName;
}
