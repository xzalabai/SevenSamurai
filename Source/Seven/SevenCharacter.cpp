#include "SevenCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "AICharacter.h"
#include "Weapon.h"
#include "Shield.h"
#include "Combo.h"	
#include "MotionWarpingComponent.h"
#include "AnimationsDA.h"
#include "EnhancedInputSubsystems.h"
#include "AnimationComponent.h"
#include "Kismet\GameplayStatics.h"
#include "Kismet\KismetMathLibrary.h"
#include "AttributesComponent.h"
#include "AttackComponent.h"
#include "SevenPlayerController.h"
#include "GameController.h"
#include "SevenGameMode.h"

ASevenCharacter::ASevenCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	VictimDesiredPosition = CreateDefaultSubobject<USceneComponent>(TEXT("VictimDesiredPosition"));
	VictimDesiredPosition->SetupAttachment(RootComponent);

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// COMPONENTS
	AC_Animation = CreateDefaultSubobject<UAnimationComponent>(TEXT("AC_Animation"));
	AC_Attribute = CreateDefaultSubobject<UAttributesComponent>(TEXT("AC_Attribute"));
	AC_AttackComponent = CreateDefaultSubobject<UAttackComponent>(TEXT("AC_Attack"));
	AC_MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	AC_AICharacter = CreateDefaultSubobject<UAICharacter>(TEXT("AICharacterComponent"));

	//AC_AttackComponent->RegisterComponent();
}

void ASevenCharacter::BeginPlay()
{
	Super::BeginPlay();
	check(WeaponType);

	EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponType);
	USkeletalMeshComponent* PlayerMesh = GetMesh();

	if (EquippedWeapon)
	{
		EquippedWeapon->AttachToSocket(PlayerMesh, "hand_rSocket");
	}
	if (SevenCharacterType == ESevenCharacterType::Lancet)
	{
		EquippedShield = GetWorld()->SpawnActor<AShield>(ShieldType);
		EquippedShield->AttachToSocket(PlayerMesh, "hand_lSocket");
	}
	
	uniqueID = UniqueIDCounter++;

	SevenGameMode = Cast<ASevenGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	SevenGameMode->UpdateStatus(this);
	UE_LOG(LogTemp, Error, TEXT("[ASevenCharacter] BeginPlay for %s"), *GetName());
	AC_Animation->SwitchStances(EStances::Guard);
	
}

///////////////////////////////////////////////////////////////////////
/// Inputs

void ASevenCharacter::Space(const FInputActionValue& Value)
{
	Jump();
}

void ASevenCharacter::StopSpace(const FInputActionValue& Value)
{
	StopJumping();
}

void ASevenCharacter::LockTarget(const bool bEnable)
{
	AC_Animation->LockTarget(bEnable);
}

void ASevenCharacter::NextComboTriggered(bool bEnable)
{
	AC_Animation->NextComboTriggered(bEnable);
}

void ASevenCharacter::NextAttackAvailable()
{
	// only for enemy
}

void ASevenCharacter::Fire(const FInputActionValue& Value)
{
	// Order is important (because EnemyCharacter listens to UpdateStatus)
	TargetedEnemy = GetClosestEnemyInRange();

	if (AC_AttackComponent->ComboActivated != ECombo::ES_None)
	{
		AC_AttackComponent->ComboAttack();
		// Intentionally don't inform attacker about Combo (so he couldn't use predefined defend)
	}
	else
	{
		bool bLightAttackPlaying = AC_AttackComponent->LightAttack(TargetedEnemy);
		if (bLightAttackPlaying)
		{
			SevenGameMode->UpdateStatus(this, ECharacterState::IncomingAttack);
		}
	}
}

void ASevenCharacter::Block(bool bEnable)
{
	if (!AC_Animation)
	{
		// TODO: Fix: This is due to error when entered the battle and enemy immediately want's to defend (and nothing is ready yet)
		return;
	}
	if (!bEnable && AC_Animation->Stance == EStances::Run)
	{
		// don't switch when started sprinting during block (and still hold finger)
		return;
	}
	
	AC_Animation->SwitchStances(bEnable ? EStances::Block : EStances::Guard);

	if (bEnable)
	{
		CheckIfBlockingBeforeParrying();
	}
	else
	{
		bIsBlockingBeforeAttack = false;
	}
}

void ASevenCharacter::Guard(bool bEnable)
{
	//AC_Animation->Guard(bEnable);
}

void ASevenCharacter::Run(bool bEnable)
{
	AC_Animation->SwitchStances(bEnable ? EStances::Run : EStances::Guard);
}

void ASevenCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		//const FRotator Rotation = GetActorRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASevenCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASevenCharacter::Special(int ID)
{
	AC_AttackComponent->SetCombo(ID);
}

void ASevenCharacter::FireRMB(const ETriggerEvent& TriggerEvent)
{

}

/// Inputs
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
/// Callbacks from ABP

void ASevenCharacter::AttackStart()
{
	// This is called also when a combo attack has a subsequence .
	EquippedWeapon->AttackStart();
}

void ASevenCharacter::AttackEnd()
{
	SevenGameMode->UpdateStatus(this, ECharacterState::AttackEnd); // TODO: this might not work with a lot of Characters ... race condition!!!
	AC_AttackComponent->bAttackCanBreakBlock = false;
	TargetedEnemy = nullptr;
	bIsImmortal = false;
	OnAttackEnd.Broadcast();
}

void ASevenCharacter::ComboAttackStart()
{
	AC_AttackComponent->ComboAttackStart();
}

void ASevenCharacter::ComboAttackEnd()
{
	AC_AttackComponent->ComboAttackEnd();
	AttackEnd();
}

void ASevenCharacter::AttackWasParried()
{
	UE_LOG(LogTemp, Error, TEXT("[ASevenCharacter] AttackWasParried"));
	AttackEnd();
	AC_Animation->Play(Animations->Montages[EMontageType::Parry], "1", EMontageType::HitReaction);
}

void ASevenCharacter::PerformWeaponTrace()
{
	EquippedWeapon->PerformTrace();
}

void ASevenCharacter::AttackCanBreakBlock()
{
	//AC_AttackComponent->bAttackCanBreakBlock = true;
}

/// Callbacks from ABP
///////////////////////////////////////////////////////////////////////

int8 ASevenCharacter::GetMappedComboKey(const EComboType& ComboType) const
{
	for (const TPair<int8, UObject* >& ComboUObject : AC_AttackComponent->CombosMapping)
	{
		IComboInterface* Combo = Cast<IComboInterface>(ComboUObject.Value);
		if (ComboType == Combo->GetComboType())
		{
			return ComboUObject.Key;
		}
	}
	UE_LOG(LogTemp, Fatal, TEXT("[ASevenCharacter] GetMappedComboKey Unable to map Combo: %d"), (int) ComboType);
	return -1;
}

FAttackInfo ASevenCharacter::GetAttackInfo() const
{
	return AC_AttackComponent->GetAttackInfo();
}

bool ASevenCharacter::CanParryAttack(const ASevenCharacter* Attacker) const
{
	if (!GetIsBlocking())
	{
		return false;
	}
	if (IsBlockingBeforeAttack())
	{
		return false;
	}
	if (GetEnemiesInFrontOfCharacer(Attacker->GetUniqueID()).IsEmpty())
	{	
		return false;
	}
	return true;
}

void ASevenCharacter::OnLayingDead()
{
	SevenGameMode->UpdateStatus(this, ECharacterState::Dead);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetGenerateOverlapEvents(false);
	if (Controller)
	{
		Controller->UnPossess();
	}
}

UAnimMontage* ASevenCharacter::GetVictimMontageToPlay(bool bDeath, const ESevenCharacterType& AttackerCharacterType, const EMontageType& AttackerMontageType, const EComboType& ComboType) const
{
	UAnimMontage* MontageToPlay{ nullptr };
	if (bDeath)
	{
		switch (AttackerMontageType)
		{
		case EMontageType::LightAttack:
			MontageToPlay = Animations->Reactions[AttackerCharacterType].AnimationMapping[EMontageType::LightAttackHitReactionDeath];
			break;
		case EMontageType::Combo:
			MontageToPlay = Animations->ComboAnimations->ComboDeathReactions[ComboType];
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("[ASevenCharacter] Unresolved attack type: %d"), (int)AttackerMontageType);
			check(1 == 0);
			return nullptr;
		}
	}
	else
	{
		switch (AttackerMontageType)
		{
		case EMontageType::LightAttack:
			MontageToPlay = Animations->Reactions[AttackerCharacterType].AnimationMapping[EMontageType::LightAttackHitReaction];
			break;
		case EMontageType::Combo:
			MontageToPlay = Animations->ComboAnimations->ComboHitReactions[ComboType];
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("[ASevenCharacter] Unresolved attack type: %d"), (int)AttackerMontageType);
			check(1 == 0);
			return nullptr;
		}
	}
	return MontageToPlay;
}

void ASevenCharacter::Suicide()
{
	UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter] Suicide CHEAT"));
	UAnimMontage* MontageToPlay = Animations->Reactions[SevenCharacterType].AnimationMapping[EMontageType::LightAttackHitReactionDeath];

	// TODO: For now, random receivedHit animation is being played
	int RandomMontage = FMath::RandRange(1, MontageToPlay->CompositeSections.Num());
	AC_Animation->Play(MontageToPlay, CustomMath::IntToFName(RandomMontage), EMontageType::LightAttackHitReaction);
}

void ASevenCharacter::ReceivedHit(const FAttackInfo& AttackInfo)
{
	if (bDebugIsImmortal)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ASevenCharacter].ReceivedHit %s, but bDebugIsImmortal"), *GetName());
		return;
	}

	if (!IsAlive())
	{
		return;
	}
	
	if (IsSameTeam(AttackInfo.Attacker))
	{
		return;
	}

	if (IsImmortal())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ASevenCharacter].ReceivedHit %s, but Immortal"), *GetName());
		return;
	}

	const EReceivedHitReaction ReceivedHitReaction = GetHitReaction(AttackInfo);
	UE_LOG(LogTemp, Warning, TEXT("[ASevenCharacter] Character %s ReceivedHitReaction: %d"), *GetName(), (int)ReceivedHitReaction);
	
	if (ReceivedHitReaction == EReceivedHitReaction::Parried)
	{
		AC_Animation->Play(Animations->Montages[EMontageType::Parry], "0", EMontageType::Parry);
		AttackInfo.Attacker->AttackWasParried();
		UE_LOG(LogTemp, Warning, TEXT("[ASevenCharacter]. Attack Parried"), *GetName());
		bIsImmortal = true;
		AC_Attribute->Add(EItemType::XP, 10);
		return;
	}

	if (ReceivedHitReaction == EReceivedHitReaction::Blocked)
	{
		if (SevenCharacterType == ESevenCharacterType::Lancet)
		{
			// Shield
			AttackInfo.Attacker->AttackWasParried();
			return;
		}
		else
		{
			const FName RandomMontageStr = CustomMath::GetRandomNumber_FName(0, Animations->Montages[EMontageType::Block]->CompositeSections.Num() - 1);
			AC_Animation->Play(Animations->Montages[EMontageType::Block], RandomMontageStr, EMontageType::Block);
			return;
		}
	}

	if (ReceivedHitReaction == EReceivedHitReaction::BlockBroken)
	{
		AC_Animation->SwitchStances(EStances::Guard);
		AC_Animation->Play(Animations->Montages[EMontageType::BlockBroken], "1", EMontageType::BlockBroken);
		return;
	}

	if (ReceivedHitReaction == EReceivedHitReaction::Evaded)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ASevenCharacter]. Attack Evaded"));
		bIsImmortal = true;
		AC_Attribute->Add(EItemType::XP, 10);
		return;
	}

	bool bDead = ReceivedHitReaction == EReceivedHitReaction::Dead ? true : false;
	UAnimMontage* MontageToPlay = GetVictimMontageToPlay(bDead, AttackInfo.Attacker->GetSevenCharacterType(), AttackInfo.MontageType, AttackInfo.ComboType);
	int RandomMontage = FMath::RandRange(1, MontageToPlay->CompositeSections.Num());
	AC_Animation->Play(MontageToPlay, CustomMath::IntToFName(RandomMontage), EMontageType::HitReaction);
}

bool ASevenCharacter::IsSameTeam(const ASevenCharacter* Other) const
{
	return Other->IsEnemy() == IsEnemy();
}

bool ASevenCharacter::IsAlive() const
{
	return AC_Attribute->GetHP() > 0;
}

void ASevenCharacter::AI_MoveToPosition(const FVector& Position)
{
	AC_AICharacter->MoveTo(Position);
}

void ASevenCharacter::Evade(const FInputActionValue& Value)
{
	if (AC_Animation->Play(Animations->Montages[EMontageType::Evade], (int)GetDirection(Value.Get<FVector2D>()), EMontageType::Evade))
	{
		bIsEvading = true;
		UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter] Evade"));
	}
}

void ASevenCharacter::CheckIfBlockingBeforeParrying()
{
	if (SevenGameMode->HasAnyEnemyStatus(ECharacterState::ParryAvailable))
	{
		bIsBlockingBeforeAttack = false;
	}
	else
	{
		bIsBlockingBeforeAttack = true;
	}
}

TArray<ASevenCharacter*> ASevenCharacter::GetEnemiesInFrontOfCharacer(const int8 EnemyID, const int32 StartOffset, const int32 EndOffset, const int32 Thickness, const bool bCameraRelative) const
{
	 TArray<ASevenCharacter*> FoundActors;
	 TArray<FHitResult> HitResults;
	 FVector DirectionOfSphere = bEnemy || !bCameraRelative ? GetActorForwardVector() : FollowCamera->GetForwardVector();
	 DirectionOfSphere.Z = 0;
	 
	 const FVector Start = GetActorLocation() + DirectionOfSphere * StartOffset;
	 const FVector End = GetActorLocation() + DirectionOfSphere * EndOffset;

	 bool bHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, Thickness, UEngineTypes::ConvertToTraceType(ECC_WorldDynamic), false, TArray<AActor*>{}, EDrawDebugTrace::None, HitResults, true);

	 for (FHitResult& HitResult : HitResults)
 	 {
 		 if (ASevenCharacter* Enemy = Cast<ASevenCharacter>(HitResult.GetActor()))
 		 {
			 if (!Enemy->IsAlive())
			 {
				 // dead
				 continue;
			 }

			 if (Enemy->GetUniqueID() == GetUniqueID())
			 {
				 // self
				 continue;
			 }

			 if (Enemy->IsEnemy() == IsEnemy())
			 {
				 // teamnate
				 continue;
			 }

			 if (FoundActors.Contains(Enemy))
			 {
				 // already included
				 continue;
			 }

 			 if (EnemyID == -1 || Enemy->GetUniqueID() == EnemyID)
 			 {
 				 FoundActors.Add(Enemy);
 			 }
 		 }
 	 }
	 return FoundActors;
}

ASevenCharacter* ASevenCharacter::GetClosestEnemyInRange(float DotProductTreshold)
{
	TArray<ASevenCharacter*> FoundEnemies = GetEnemiesInFrontOfCharacer();
	
	if (FoundEnemies.Num() > 0)
	{
		return FoundEnemies[0];
	}
	return nullptr;
}

EOctagonalDirection ASevenCharacter::GetDirection(const FVector2D& Vector) const
{
	if (Vector.X == 0)
	{
		return Vector.Y == 1 ? EOctagonalDirection::Forward : EOctagonalDirection::Backward;
	}
	if (Vector.Y == 0)
	{
		return Vector.X == 1 ? EOctagonalDirection::Right : EOctagonalDirection::Left;
	}

	return EOctagonalDirection::None;
}

void ASevenCharacter::OnAnimationEnded(const EMontageType& StoppedMontage)
{
	AC_MotionWarpingComponent->RemoveWarpTarget("MW_LightAttackAttacker");
	bIsImmortal = false;
	bIsBlockingBeforeAttack = true;
}

bool ASevenCharacter::IsEvadingAway(const ASevenCharacter* Enemy) const
{
	if (!bIsEvading)
	{
		return false;
	}

	EOctagonalDirection EvadeDirection = OctagonalDirection::GetOctagonalDirectionFName(AC_Animation->GetCurrentMontageSection());
	//EOctagonalDirection EvadeDirection = EOctagonalDirection::None;
	UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter] ReceivedHit.IsEvadingAway %d"), EvadeDirection);
	const float Dot = FVector::DotProduct(GetActorForwardVector().GetSafeNormal(), Enemy->GetActorForwardVector().GetSafeNormal());
	const float Cross = FVector::CrossProduct(GetActorForwardVector().GetSafeNormal(), Enemy->GetActorForwardVector().GetSafeNormal()).Z;


	// DEBUG
	
	FVector StartLocation(0, 0, 0);
	FVector EndLocation(100, 0, 0);
	FColor DebugColor(255, 0, 0); // Red color
	FColor DebugColor2(0, 255, 0); // Red color
	float ArrowSize = 10.0f;
	float Thickness = 2.0f;
	float Duration = 5.0f;
	uint8 DepthPriority = 0; // Default depth priority
	float Length = -1.0f; // Length of the arrow shaft, -1 to use default
	//DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector().GetSafeNormal() * 1000, ArrowSize, DebugColor, false, Duration, DepthPriority, Thickness);
	//DrawDebugDirectionalArrow(GetWorld(), Enemy->GetActorLocation(), Enemy->GetActorLocation() + Enemy->GetActorForwardVector().GetSafeNormal() * 1000, ArrowSize, DebugColor2, false, Duration, DepthPriority, Thickness);
	//DrawDebugPoint(GetWorld(), T.GetTranslation(), ArrowSize, DebugColor2, true, Duration, DepthPriority);

	if (Cross > 0) // ^ ^ || < ^
	{
		if (Dot >= 0.5f) // ^ ^
		{
			UE_LOG(LogTemp, Warning, TEXT("^ ^"));
			return (EvadeDirection != EOctagonalDirection::Backward && EvadeDirection != EOctagonalDirection::BackwardLeft && EvadeDirection != EOctagonalDirection::BackwardRight);
		}
		else // < ^
		{
			UE_LOG(LogTemp, Warning, TEXT("< ^"));
			return (EvadeDirection != EOctagonalDirection::Left && EvadeDirection != EOctagonalDirection::ForwardLeft && EvadeDirection != EOctagonalDirection::BackwardLeft);
		}
	}
	else if (Cross < 0) // v ^ || > ^ 
	{
		if (-0.7f <= Dot && Dot <= 0.7f)
		{
			UE_LOG(LogTemp, Warning, TEXT("> ^ "));
			return (EvadeDirection != EOctagonalDirection::Right && EvadeDirection != EOctagonalDirection::ForwardRight && EvadeDirection != EOctagonalDirection::BackwardRight);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("v ^ "));
			return (EvadeDirection != EOctagonalDirection::Forward && EvadeDirection != EOctagonalDirection::ForwardLeft && EvadeDirection != EOctagonalDirection::ForwardRight);
		}
	}
	return false;
}

void ASevenCharacter::RotateTowards(const AActor* Actor, const int Shift)
{
	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Actor->GetActorLocation());
	RootComponent->SetWorldRotation(PlayerRot);

	if (Shift != 0)
	{
		SetActorLocation(GetActorLocation() + GetActorForwardVector() * Shift);
	}
}

EReceivedHitReaction ASevenCharacter::GetHitReaction(const FAttackInfo& AttackInfo) const
{
	if (AttackInfo.AttackStrength == EAttackStrength::Undefendable)
	{
		return GetSuccessfulHitReaction(AttackInfo.Damage);
	}
	
	if (IsAllowedHitReaction(AttackInfo.AttackStrength, EAttackStrength::CanParry) && CanParryAttack(AttackInfo.Attacker))
	{
		return EReceivedHitReaction::Parried;
	}

	if (IsAllowedHitReaction(AttackInfo.AttackStrength, EAttackStrength::CanBlock) && GetIsBlocking())
	{
		if (!GetEnemiesInFrontOfCharacer(AttackInfo.Attacker->GetUniqueID()).IsEmpty())
		{
			if (AttackInfo.bAttackCanBreakBlock)
			{
				return EReceivedHitReaction::BlockBroken;
			}
			else
			{
				return EReceivedHitReaction::Blocked;
			}
		}
	}

	if (IsAllowedHitReaction(AttackInfo.AttackStrength, EAttackStrength::CanEvade) && IsEvadingAway(AttackInfo.Attacker))
	{
		return EReceivedHitReaction::Evaded;
	}

	return GetSuccessfulHitReaction(AttackInfo.Damage);
}

void ASevenCharacter::StealAttackToken(const uint8 enemyUniqueID)
{
	AttackToken = enemyUniqueID;
}

void ASevenCharacter::ResetAttackToken()
{
	AttackToken = 0;
}

EReceivedHitReaction ASevenCharacter::GetSuccessfulHitReaction(const uint8 Damage) const
{
	return AC_Attribute->GetHP() - Damage > 0 ? EReceivedHitReaction::Hit : EReceivedHitReaction::Dead;
}

ASevenPlayerController* ASevenCharacter::GetSevenPlayerController() const
{
	ASevenPlayerController* SevenPlayerController = Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	return SevenPlayerController;
}