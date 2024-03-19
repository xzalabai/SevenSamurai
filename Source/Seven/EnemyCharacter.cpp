#include "EnemyCharacter.h"
#include "Weapon.h"
#include "AnimationComponent.h"
#include "SevenPlayerController.h"
#include "ComboManager.h"
#include "AttackComponent.h"
#include "Kismet\GameplayStatics.h"
#include "Kismet\KismetMathLibrary.h"

void AEnemyCharacter::InitiateAttack()
{
	Fire(FInputActionValue());
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	SevenPlayerController = Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	SevenPlayerController->UpdateStatus(GetUniqueID());
}

void AEnemyCharacter::IncomingAttack()
{
	if (SevenPlayerController)
	{
		SevenPlayerController->UpdateStatus(GetUniqueID(), EEnemyStatus::IncomingAttack);
	}
}

void AEnemyCharacter::ParryAvailable(bool bEnable)
{
	if (SevenPlayerController)
	{
		SevenPlayerController->UpdateStatus(GetUniqueID(), bEnable ? EEnemyStatus::ParryAvailable : EEnemyStatus::ParryUnavailable);
	}
}

void AEnemyCharacter::Fire(const FInputActionValue& Value)
{
	//Attack
	
	TargetedEnemy = GetClosestEnemyInRange();
	AC_AttackComponent->LightAttack(TargetedEnemy);

	if (TargetedEnemy)
	{
		UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter]Fire.TargetedEnemy %s"), *TargetedEnemy->GetName());
		//MotionWarpingComponent->AddOrUpdateWarpTargetFromTransform("MW_LightAttackAttacker", TargetedEnemy->VictimDesiredPosition->GetComponentTransform());

		// Rotate character towards enemy
		FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetedEnemy->GetActorLocation());
		RootComponent->SetWorldRotation(PlayerRot);

		if (!AC_AttackComponent->LightAttack(TargetedEnemy))
		{
			AttackEnd();
		}
	}
	
}

void AEnemyCharacter::AttackEnd() const
{
	UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter] AttackEnd"));
	OnAttackEnd.Broadcast();
	SevenPlayerController->UpdateStatus(GetUniqueID(), EEnemyStatus::Cooldown);
}

