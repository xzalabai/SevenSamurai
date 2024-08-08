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
	else
	{
		AttackEnd();
	}

	ReturnAttackToken();
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

void AEnemyCharacter::OnSevenCharacterStatusUpdate(const AActor* Actor, const ECharacterState Status)
{
	const ASevenCharacter* const SevenCharacter = Cast<ASevenCharacter>(Actor);

	if (Status == ECharacterState::IncomingAttack || Status == ECharacterState::AttackEnd)
	{
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
	SetDefendReactionInProgress();
}

void AEnemyCharacter::ReceivedHit(const FAttackInfo& AttackInfo)
{
	SetDefendReactionInProgress();
	Super::ReceivedHit(AttackInfo);
	ReturnAttackToken();
}

void AEnemyCharacter::OnLayingDead()
{
	Super::OnLayingDead();
}

void AEnemyCharacter::SetDefendReactionInProgress() const
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return;
	}

	UBlackboardComponent* BlackBoardComponent = AIController->GetBlackboardComponent();
	
	if (!BlackBoardComponent)
	{
		return;
	}
	FName BBValue = FName(TEXT("bDefendReactionInProgress"));
	bool const bPreviousValue = BlackBoardComponent->GetValueAsBool(BBValue);
	bool const bNewValue = AC_Animation->IsDefendReactionInProgress();
	if (bPreviousValue == bNewValue)
	{
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("[AEnemyCharacter] Set Defend %d"), bNewValue ? 1 : 0);

	BlackBoardComponent->SetValueAsBool(BBValue, bNewValue);
	
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
	if (ASevenCharacter* EnemyToAttack = FindSevenCharacter())
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
	//check(EnemyScenarios->EnemyScenarios.Contains(MissionType))
	return EnemyScenarios->EnemyScenarios[SevenCharacterType];
}

void AEnemyCharacter::UseCombo(const EComboType ComboType)
{
	AC_AttackComponent->SetCombo(1);
	AC_AttackComponent->LightAttack(nullptr);
	ReturnAttackToken();
}

void AEnemyCharacter::SetAttackStrength(EAttackStrength NewAttackStrength)
{
	AttackStrength = NewAttackStrength;
}

const EAttackStrength AEnemyCharacter::GetAttackStrength() const
{
	return AttackStrength;
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





