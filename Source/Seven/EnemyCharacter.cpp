#include "EnemyCharacter.h"
#include "Weapon.h"
#include "AnimationComponent.h"
#include "EnemyScenarios.h"
#include "AICharacter.h"
#include "Particles/ParticleSystem.h"
#include "AttackComponent.h"
#include "Kismet\GameplayStatics.h"
#include "Kismet\KismetMathLibrary.h"
#include <AIController.h>
#include "GameController.h"

AEnemyCharacter::AEnemyCharacter()
{
	IncomingAttackParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("ImpactParticles"));
}

void AEnemyCharacter::InitiateAttack()
{
	AC_AICharacter->Fire();
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(EnemyScenarios);
	check(MissionType != EMissionType::NotProvided);
}

void AEnemyCharacter::IncomingAttack()
{
	const UGameInstance* GameInstance = Cast<UGameInstance>(GetWorld()->GetGameInstance());
	UGameController* GameController = Cast<UGameController>(GameInstance->GetSubsystem<UGameController>());
	GameController->UpdateStatus(this, EEnemyStatus::IncomingAttack);

	if (ASevenCharacter* EnemyToAttack = AC_AICharacter->SelectEnemy())
	{
		// Spawn emitter only if really attacking (enemy has a token)
		if (EnemyToAttack->GetAttackTokenOwner() == uniqueID)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), IncomingAttackParticle,
				GetActorLocation(), FRotator(0, 0, 0), FVector(1, 1, 1), true, EPSCPoolMethod::None, true);
		}
	}

}

void AEnemyCharacter::ParryAvailable(bool bEnable)
{
	const UGameInstance* GameInstance = Cast<UGameInstance>(GetWorld()->GetGameInstance());
	UGameController* GameController = Cast<UGameController>(GameInstance->GetSubsystem<UGameController>());
	GameController->UpdateStatus(this, bEnable ? EEnemyStatus::ParryAvailable : EEnemyStatus::ParryUnavailable);
}

void AEnemyCharacter::ReceivedHit(const FAttackInfo& AttackInfo)
{
	UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter]ReceivedHit"));
	Super::ReceivedHit(AttackInfo);
	ReturnAttackToken();
}

void AEnemyCharacter::AttackEnd() const
{
	UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter] AttackEnd"));
	OnAttackEnd.Broadcast();

	const UGameInstance* GameInstance = Cast<UGameInstance>(GetWorld()->GetGameInstance());
	UGameController* GameController = Cast<UGameController>(GameInstance->GetSubsystem<UGameController>());
	GameController->UpdateStatus(this, EEnemyStatus::Cooldown);

}

void AEnemyCharacter::OnLayingDead()
{
	const UGameInstance* GameInstance = Cast<UGameInstance>(GetWorld()->GetGameInstance());
	UGameController* GameController = Cast<UGameController>(GameInstance->GetSubsystem<UGameController>());
	GameController->UpdateStatus(this, EEnemyStatus::Dead);

	Super::OnLayingDead();
}

void AEnemyCharacter::MoveTo(bool bToSevenCharacter, bool bBlockingStance)
{
	AC_AICharacter->MoveTo(bToSevenCharacter, bBlockingStance);
}

bool AEnemyCharacter::TryStealAttackToken()
{
	// TODO: This will be changed to a different logic (when there are more players) keep getting SevenCharacter from SevenPlayerController for now
	if (ASevenCharacter* EnemyToAttack = AC_AICharacter->SelectEnemy())
	{
		if (EnemyToAttack->CanStealAttackToken())
		{
			EnemyToAttack->StealAttackToken(uniqueID);
			SevenCharacterToAttack = EnemyToAttack;
			return true;
		}
	}
	return false;
}

UBehaviorTree* AEnemyCharacter::GetBehaviorTree() const
{
	check(EnemyScenarios->EnemyScenarios.Contains(MissionType))
	return EnemyScenarios->EnemyScenarios[MissionType];
}

void AEnemyCharacter::ReturnAttackToken()
{
	if (SevenCharacterToAttack)
	{
		UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter].ReturnAttackToken Returning Attack Token"));
		SevenCharacterToAttack->ResetAttackToken();
		SevenCharacterToAttack = nullptr;
	}
}





