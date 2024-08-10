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
	EasyAttackParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("EasyImpactParticles"));
	MidAttackParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("MidImpactParticles"));
	HeavyAttackParticle = CreateDefaultSubobject<UParticleSystem>(TEXT("HeavyImpactParticles"));
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	for (const TSubclassOf<UObject> Combo : AvailableCombos)
	{
		AC_AttackComponent->AddComboToCharacter(Combo);
	}

	SevenGameMode->OnSevenCharacterStatusUpdate.AddUObject(this, &AEnemyCharacter::OnSevenCharacterStatusUpdate);
	check(MissionType != EMissionType::NotProvided);
}

ASevenCharacter* AEnemyCharacter::FindSevenCharacter() const
{
	return SevenGameMode->GetPossessedCharacter();
}

void AEnemyCharacter::Fire(const FInputActionValue& Value)
{
	//Attack
	TargetedEnemy = FindSevenCharacter();
	if (TargetedEnemy)
	{
		RotateTowards(TargetedEnemy);
		bool bLightAttackPlaying = AC_AttackComponent->LightAttack(TargetedEnemy);
		if (!bLightAttackPlaying)
		{
			AttackEnd();
		}
	}
	else
	{
		AttackEnd();
	}
}

void AEnemyCharacter::AttackEnd()
{
	ReturnAttackToken();
	Super::AttackEnd();
}

void AEnemyCharacter::InitiateAttack()
{
	LightAttacksAmount--;
	if (LightAttacksAmount >= 0)
	{
		Fire(FInputActionValue());
	}
}

void AEnemyCharacter::IncomingAttack()
{
	SevenGameMode->UpdateStatus(this, ECharacterState::IncomingAttack);

	if (ASevenCharacter* EnemyToAttack = FindSevenCharacter())
	{
		// Spawn emitter only if really attacking (enemy has a token)
		if (EnemyToAttack->GetAttackTokenOwner() == uniqueID)
		{
			switch (AttackStrength)
			{
				// TODO: Reuse this: Object pool pls!
				case EAttackStrength::Light:
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EasyAttackParticle,
						GetActorLocation(), FRotator(0, 0, 0), FVector(1, 1, 1), true, EPSCPoolMethod::None, true);
					break;
				case EAttackStrength::Mid:
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MidAttackParticle,
						GetActorLocation(), FRotator(0, 0, 0), FVector(1, 1, 1), true, EPSCPoolMethod::None, true);
					break;
				case EAttackStrength::Heavy:
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HeavyAttackParticle,
						GetActorLocation(), FRotator(0, 0, 0), FVector(1, 1, 1), true, EPSCPoolMethod::None, true);
					break;
			}

		}
	}
}

void AEnemyCharacter::ParryAvailable(bool bEnable)
{
	UE_LOG(LogTemp, Error, TEXT("[AEnemyCharacter] ParryAvailable %d"), bEnable ? 1 : 0);
	SevenGameMode->UpdateStatus(this, bEnable ? ECharacterState::ParryAvailable : ECharacterState::ParryUnavailable);
}

void AEnemyCharacter::OnSevenCharacterStatusUpdate(const ASevenCharacter* SevenCharacter, const ECharacterState Status)
{
	if (Status == ECharacterState::IncomingAttack || Status == ECharacterState::AttackEnd)
	{
		//UE_LOG(LogTemp, Error, TEXT("[AEnemyCharacter] OnSevenCharacterStatusUpdate: MY ID: %d, attack ID %d"), uniqueID, SevenCharacter->GetTargetedEnemyID());
		if (SevenCharacter->GetTargetedEnemyID() == uniqueID)
		{
			AAIController* AIController = Cast<AAIController>(GetController());
			UBlackboardComponent* BlackBoardComponent = AIController->GetBlackboardComponent();
			BlackBoardComponent->SetValueAsBool(TEXT("bPlayerIncomingAttack"), Status == ECharacterState::IncomingAttack ? true : false);
		}
	}
}

void AEnemyCharacter::OnAnimationEnded(const EMontageType& StoppedMontage)
{
	Super::OnAnimationEnded(StoppedMontage);
	SetDefendActionInProgress(false); // In case of Evade
}

void AEnemyCharacter::ReceivedHit(const FAttackInfo& AttackInfo)
{
	Super::ReceivedHit(AttackInfo);
}

bool AEnemyCharacter::DefendAgainstIncomingAttack(EMontageType DefendMontage)
{
	SetDefendActionInProgress(true);
	DefendActionResolved();

	if (!AC_Animation->CanPlayAnimation(DefendMontage) || DefendMontage == EMontageType::None)
	{
		return false;
	}

	if (HasAttackStarted())
	{
		return false;
	}

	if (DefendMontage == EMontageType::Block)
	{
		Block(true);
		return true;
	}
	else if (DefendMontage == EMontageType::Evade)
	{
		const TArray<FInputActionValue::Axis2D> PossibleEvades = { FInputActionValue::Axis2D(0,-1),  FInputActionValue::Axis2D(-1, 0), FInputActionValue::Axis2D(1,0) };
		const int RandomEvadeIndex = FMath::RandRange(0, PossibleEvades.Num() - 1);
		Evade(PossibleEvades[RandomEvadeIndex]);
		return true;
	}
	else
	{
		return false;
	}
	return false;
}

void AEnemyCharacter::SetDefendActionInProgress(const bool bInProgress) const
{
	//UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter] SetDefendActionInProgress %d"), bInProgress ? 1 : 0);
	AAIController* AIController = Cast<AAIController>(GetController());
	UBlackboardComponent* BlackBoardComponent = AIController->GetBlackboardComponent();

	const FName BBValue{ TEXT("bDefendReactionInProgress") };
	bool const bPreviousValue = BlackBoardComponent->GetValueAsBool(BBValue);
	BlackBoardComponent->SetValueAsBool(BBValue, bInProgress);
}

void AEnemyCharacter::MoveTo(bool bToSevenCharacter)
{	
	AAIController* AIController = Cast<AAIController>(GetController());
	ASevenCharacter* EnemyToAttack = FindSevenCharacter();

	if (!AIController || !EnemyToAttack)
	{
		return;
	}

	FVector FinalDestination = bToSevenCharacter ? EnemyToAttack->GetActorLocation() : GetRandomPointAroundCharacter(EnemyToAttack);
	AIController->SetFocus(EnemyToAttack);
	AIController->SetMoveBlockDetection(false);
	AIController->MoveToLocation(FinalDestination, 80.0f);
}

const FVector AEnemyCharacter::GetRandomPointAroundCharacter(const ASevenCharacter* const SevenCharacter)
{
	const FVector EnemyPosition = SevenCharacter->GetActorLocation();
	const FVector EnemyForwardVector = SevenCharacter->GetActorForwardVector();

	const int Left = FMath::RandBool() ? (-1) : 1;
	//const int RandomOffsetX = FMath::RandRange(500, 1000);
	const int RandomOffsetX = FMath::RandRange(20, 500);
	const int RandomOffsetY = FMath::RandRange(20, 500) * Left;

	const FVector EnemyForwardVectorOffset = FVector(EnemyForwardVector.X + RandomOffsetX, EnemyForwardVector.Y + RandomOffsetY, EnemyForwardVector.Z);

	const FVector FinalDestination = FVector(EnemyPosition.X + EnemyForwardVectorOffset.X, EnemyPosition.Y + EnemyForwardVectorOffset.Y, EnemyPosition.Z);

	return FinalDestination;
}

bool AEnemyCharacter::TryStealAttackToken()
{
	if (ASevenCharacter* const EnemyToAttack = FindSevenCharacter())
	{
		if (EnemyToAttack->CanStealAttackToken() || EnemyToAttack->GetAttackTokenOwner() == uniqueID)
		{
			EnemyToAttack->StealAttackToken(uniqueID);
			TargetedEnemy = EnemyToAttack;
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

void AEnemyCharacter::SetLightAttacksAmount(int Amount)
{
	LightAttacksAmount = Amount;
}

UBehaviorTree* AEnemyCharacter::GetBehaviorTree() const
{
	return EnemyScenarios->EnemyScenarios[SevenCharacterType];
}

void AEnemyCharacter::UseCombo(const EComboType ComboType)
{
	check(ComboType != EComboType::None);
	Special(GetMappedComboKey(ComboType));
	AC_AttackComponent->ComboAttack();
}

void AEnemyCharacter::SetAttackStrength(EAttackStrength NewAttackStrength)
{
	AttackStrength = NewAttackStrength;
}

bool AEnemyCharacter::HasAttackStarted() const
{
	if (AC_Animation->GetCurrentMontageType() == EMontageType::LightAttack || AC_Animation->GetCurrentMontageType() == EMontageType::Combo)
	{
		// Attack animation started
		return true;
	}
	if (SevenGameMode->GetEnemyStatus(uniqueID) == ECharacterState::IncomingAttack)
	{
		// Attack is imminent
		return true;
	}
	return false;
}

void AEnemyCharacter::ReturnAttackToken()
{
	if (TargetedEnemy)
	{
		TargetedEnemy->ResetAttackToken();
		TargetedEnemy = nullptr;
	}
}





