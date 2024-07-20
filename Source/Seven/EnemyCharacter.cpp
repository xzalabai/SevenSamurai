#include "EnemyCharacter.h"
#include "Weapon.h"
#include "AnimationComponent.h"
#include "EnemyScenarios.h"
#include "AICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Particles/ParticleSystem.h"
#include "AttackComponent.h"
#include "Kismet\GameplayStatics.h"
#include "Kismet\KismetMathLibrary.h"
#include "AIController.h"
#include "SevenGameMode.h"

AEnemyCharacter::AEnemyCharacter()
{
	IncomingAttackParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("ImpactParticles"));
}

void AEnemyCharacter::InitiateAttack()
{
	LightAttacksAmount--;
	if (LightAttacksAmount >= 0)
	{
		bIsImmortal = true;
		AC_AICharacter->Fire();
	}
	else
	{
		bIsImmortal = false;
	}
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	SevenGameMode->OnSevenCharacterStatusUpdate.AddUObject(this, &AEnemyCharacter::OnSevenCharacterStatusUpdate);

	check(EnemyScenarios);
	check(MissionType != EMissionType::NotProvided);
}

void AEnemyCharacter::IncomingAttack()
{
	SevenGameMode->UpdateStatus(this, EEnemyStatus::IncomingAttack);

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
	SevenGameMode->UpdateStatus(this, bEnable ? EEnemyStatus::ParryAvailable : EEnemyStatus::ParryUnavailable);
}

void AEnemyCharacter::OnSevenCharacterStatusUpdate(const AActor* Actor, const EEnemyStatus Status)
{
	const ASevenCharacter* const SevenCharacter = Cast<ASevenCharacter>(Actor);

	if (Status == EEnemyStatus::IncomingAttack || Status == EEnemyStatus::AttackEnd)
	{
		if (SevenCharacter->GetTargetedEnemyID() == uniqueID)
		{
			AAIController* AIController = Cast<AAIController>(GetController());
			UBlackboardComponent* BlackBoardComponent = AIController->GetBlackboardComponent();
			BlackBoardComponent->SetValueAsBool(TEXT("bPlayerIncomingAttack"), Status == EEnemyStatus::IncomingAttack ? true : false);
		}
	}
}

void AEnemyCharacter::ReceivedHit(const FAttackInfo& AttackInfo)
{
	Super::ReceivedHit(AttackInfo);
	ReturnAttackToken();
}

void AEnemyCharacter::AttackEnd()
{
	UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter] AttackEnd"));
	OnAttackEnd.Broadcast();
	SevenGameMode->UpdateStatus(this, EEnemyStatus::AttackEnd);
}

void AEnemyCharacter::OnLayingDead()
{
	Super::OnLayingDead();
}

void AEnemyCharacter::MoveTo(bool bToSevenCharacter, bool bBlockingStance)
{
	AC_AICharacter->MoveTo(bToSevenCharacter, bBlockingStance);
}

bool AEnemyCharacter::TryStealAttackToken()
{
	if (ASevenCharacter* EnemyToAttack = AC_AICharacter->SelectEnemy())
	{
		if (EnemyToAttack->CanStealAttackToken() || EnemyToAttack->GetAttackTokenOwner() == uniqueID)
		{
			EnemyToAttack->StealAttackToken(uniqueID);
			SevenCharacterToAttack = EnemyToAttack;
			return true;
		}
	}
	return false;
}

void AEnemyCharacter::NextAttackAvailable()
{
	InitiateAttack();
}

void AEnemyCharacter::DefendActionResolved()
{
	AAIController* AIController = Cast<AAIController>(GetController());
	UBlackboardComponent* BlackBoardComponent = AIController->GetBlackboardComponent();
	BlackBoardComponent->SetValueAsBool(TEXT("bPlayerIncomingAttack"), false);
}

void AEnemyCharacter::PerformEvade()
{
	// TODO: pls change it to be more efficient :D 
	const TArray<FInputActionValue::Axis2D> PossibleEvades = { FInputActionValue::Axis2D(0,-1),  FInputActionValue::Axis2D(-1, 0), FInputActionValue::Axis2D(1,0)};
	const int RandomEvadeIndex = FMath::RandRange(0, PossibleEvades.Num() - 1);

	Super::Evade(PossibleEvades[RandomEvadeIndex]);
	
	// TODO: find a better way on how to forbid next evading (since loop goes there again.
}

void AEnemyCharacter::SetLightAttacksAmount(int Amount)
{
	LightAttacksAmount = Amount;
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
		//UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter].ReturnAttackToken Returning Attack Token"));
		SevenCharacterToAttack->ResetAttackToken();
		SevenCharacterToAttack = nullptr;
	}
}





