#include "SevenCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "Weapon.h"
#include "MotionWarpingComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AnimationComponent.h"
#include "Kismet\GameplayStatics.h"
#include "Kismet\KismetMathLibrary.h"
#include "AttributesComponent.h"
#include "AttackComponent.h"
#include "SevenPlayerController.h"

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
	AC_AttackComponent = CreateDefaultSubobject<UAttackComponent>(TEXT("AC_AttackComponent"));
	AC_MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	
}

void ASevenCharacter::BeginPlay()
{
	Super::BeginPlay();
	UniqueIDCounter = 0;
	uniqueID = UniqueIDCounter++; // Because of https://stackoverflow.com/questions/67414701/initializing-static-variables-in-ue4-c 
	EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponType);
	USkeletalMeshComponent* PlayerMesh = GetMesh();
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachToSocket(PlayerMesh, "hand_rSocket");
	}
	UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter] Created ASevenCharacter with ID %d"), GetUniqueID());

	AC_Attribute->SetHP(20);
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

void ASevenCharacter::Fire(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter]Fire"));

	// Attack
	TargetedEnemy = GetClosestEnemyInRange();
	AC_AttackComponent->LightAttack(TargetedEnemy);
}

void ASevenCharacter::Block(bool bEnable)
{
	UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter] Block %d"), bEnable ? 1 : 0);
	AC_Animation->Block(bEnable);

	if (bEnable)
	{
		CheckIfBlockingBeforeParrying();
	}
	else
	{
		bIsBlockingBeforeAttack = false;
	}
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
	UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter]FireRMB %d"), (uint8)TriggerEvent);

	TargetedEnemy = GetClosestEnemyInRange();
	AC_AttackComponent->HeavyAttack(TargetedEnemy, (TriggerEvent == ETriggerEvent::Completed || TriggerEvent == ETriggerEvent::Canceled));
}

/// Inputs
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
/// Callbacks from ABP

void ASevenCharacter::AttackStart()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttackStart();
	}
}

void ASevenCharacter::AttackEnd() const
{
	
}

void ASevenCharacter::ComboAttackStart()
{
	AC_AttackComponent->ComboAttackStart();
}

void ASevenCharacter::ComboAttackEnd()
{
	AC_AttackComponent->ComboAttackEnd();
}

void ASevenCharacter::AttackWasParried() const
{
	AttackEnd();
	AC_Animation->Play(ParryMontage, "1", EMontageType::Parry, true);
}

void ASevenCharacter::PerformWeaponTrace()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->PerformTrace();
	}
}

/// Callbacks from ABP
///////////////////////////////////////////////////////////////////////

bool ASevenCharacter::ParryAttack(const ASevenCharacter* Attacker)
{
	const TObjectPtr<ASevenPlayerController> SevenPlayerController = Cast<ASevenPlayerController>(Controller);
	if (GetIsBlockingBeforeAttack() || !GetIsBlocking())
	{
		return false;
	}
	if (GetEnemiesInFrontOfCharacer(Attacker->GetUniqueID()).IsEmpty())
	{
		// Is not turned towards enemy
		return false;
	}
	if (SevenPlayerController->GetEnemyStatus(Attacker->GetUniqueID()) == EEnemyStatus::ParryAvailable)
	{
		return true;
	}
	return false;
}

void ASevenCharacter::ProcessDeath()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetGenerateOverlapEvents(false);
	if (Controller)
	{
		Controller->UnPossess();
	}
}

void ASevenCharacter::ReceivedHit(const FAttackInfo &AttackInfo)
{
	const ASevenCharacter* Attacker = Cast<ASevenCharacter>(AttackInfo.Attacker);
	UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter] ReceivedHit: From %d Character"), Attacker->GetUniqueID());

	// Parry
	if (ParryAttack(Attacker))
	{
		UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter] ReceivedHit.Is Parrying"));
		AC_Animation->Play(ParryMontage, "0", EMontageType::Parry, true);
		Attacker->AttackWasParried();
		bIsImmortal = true;
		return;
	}
	
	// Block
	if (GetIsBlocking() && !GetEnemiesInFrontOfCharacer(Attacker->GetUniqueID()).IsEmpty())
	{
		UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter] ReceivedHit.Blocking"));
		const FName RandomMontageStr = CustomMath::GetRandomNumber_FName(0, BlockMontage->CompositeSections.Num());
		AC_Animation->Play(BlockMontage, RandomMontageStr, EMontageType::Block, true);
		return;
	}

	// Evade
	if (GetIsEvading() && IsEvadingAway(Cast<ASevenCharacter>(AttackInfo.Attacker)))
	{
		UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter] ReceivedHit.IsEvadingAway"));
		bIsImmortal = true;
		return;
		
	}

	const int32 NewHP = AC_Attribute->DecreaseHP(AttackInfo.Damage);
	if (NewHP == 0)
	{
		// Dead
		UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter] ReceivedHit.GetHit.HP == 0"));
		UAnimMontage* MontageToPlay = AttackInfo.AttackType == EAttackType::Light ? LightAttackVictimDeath : HeavyAttackVictimDeath; // TODO: Change

		// TODO: For now, random receivedHit animation is being played
		int RandomMontage = FMath::RandRange(1, MontageToPlay->CompositeSections.Num());
		AC_Animation->Play(MontageToPlay, CustomMath::IntToFName(RandomMontage), EMontageType::HitReaction, true);
	}
	else
	{
		// Alive
		UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter] ReceivedHit.GetHit.HP != 0"));
		UAnimMontage* MontageToPlay = AttackInfo.AttackType == EAttackType::Light ? LightAttackVictim : HeavyAttackVictim; // TODO: Change

		// TODO: For now, random receivedHit animation is being played
		int RandomMontage = FMath::RandRange(1, MontageToPlay->CompositeSections.Num());
		AC_Animation->Play(MontageToPlay, CustomMath::IntToFName(RandomMontage), EMontageType::HitReaction, true);
	}
}

bool ASevenCharacter::IsAlive() const
{
	return AC_Attribute->GetHP() > 0;
}

void ASevenCharacter::Evade(const FInputActionValue& Value)
{
	if (AC_Animation->Play(EvadeMontage, (int)GetDirection(Value.Get<FVector2D>()), EMontageType::Evade, false))
	{
		bIsEvading = true;
		UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter] Evade"));
	}
}

void ASevenCharacter::CheckIfBlockingBeforeParrying()
{
	const TObjectPtr<ASevenPlayerController> SevenPlayerController = Cast<ASevenPlayerController>(Controller);
	if (SevenPlayerController->HasAnyEnemyStatus(EEnemyStatus::ParryAvailable))
	{
		bIsBlockingBeforeAttack = false;
	}
	else
	{
		bIsBlockingBeforeAttack = true;
	}
}

TArray<ASevenCharacter*> ASevenCharacter::GetEnemiesInFrontOfCharacer(const int8 EnemyID, const int32 StartOffset, const int32 EndOffset, const int32 Thickness, const bool bCameraRelative)
{
	 TArray<ASevenCharacter*> FoundActors;
	 TArray<FHitResult> HitResults;
	 FVector DirectionOfSphere = bEnemy || !bCameraRelative ? GetActorForwardVector() : FollowCamera->GetForwardVector();
	 DirectionOfSphere.Z = 0;
	 
	 const FVector Start = GetActorLocation() + DirectionOfSphere * StartOffset;
	 const FVector End = GetActorLocation() + DirectionOfSphere * EndOffset;

	 bool bHit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, Thickness, UEngineTypes::ConvertToTraceType(ECC_WorldDynamic), false, TArray<AActor*> { this }, EDrawDebugTrace::Persistent, HitResults, true);

	 auto filter = [&]()
		 {
			 for (FHitResult& HitResult : HitResults)
			 {
				 if (ASevenCharacter* Enemy = Cast<ASevenCharacter>(HitResult.GetActor()))
				 {
					 if ((!FoundActors.Contains(Enemy)) && (EnemyID == -1 || Enemy->GetUniqueID() == EnemyID) && Enemy->IsAlive())
					 {
						 FoundActors.Add(Enemy);
					 }
				 }
			 }
		 };
	 
	 filter();
	 UE_LOG(LogTemp, Display, TEXT("ASevenCharacter.GetEnemiesInFrontOfCharacer Found Enemies %d"), FoundActors.Num());
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

void ASevenCharacter::OnAnimationEnded(const EMontageType& StoppedMontage, const EMontageType& NextMontage)
{
	TargetedEnemy = nullptr;
	AC_MotionWarpingComponent->RemoveWarpTarget("MW_LightAttackAttacker");

	if (StoppedMontage == EMontageType::Parry || StoppedMontage == EMontageType::Evade)
	{
		bIsImmortal = false;
	}
}

bool ASevenCharacter::IsEvadingAway(const ASevenCharacter* Enemy)
{
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
	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation() + GetActorForwardVector(), Actor->GetActorLocation());
	RootComponent->SetWorldRotation(PlayerRot);

	if (Shift != 0)
	{
		SetActorLocation(GetActorLocation() + GetActorForwardVector() * Shift);
	}
}





