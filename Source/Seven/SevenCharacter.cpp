#include "SevenCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "Weapon.h"
#include "EnhancedInputSubsystems.h"
#include "AnimationComponent.h"
#include <Kismet\GameplayStatics.h>
#include <Kismet\KismetMathLibrary.h>
#include "ComboManager.h"

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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	AnimationComponent = CreateDefaultSubobject<UAnimationComponent>(TEXT("AnimationComponent"));
	ComboComponent = CreateDefaultSubobject<UComboManager>(TEXT("ComboComponent"));

	//OnTakeAnyDamage.AddDynamic(this, &ASevenCharacter::TakeDamage);
}

void ASevenCharacter::BeginPlay()
{
	Super::BeginPlay();

	EquippedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponType);
	USkeletalMeshComponent* PlayerMesh = GetMesh();
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachToSocket(PlayerMesh, "hand_rSocket");
	}

	if (CanBePossessed())
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASevenCharacter::StaticClass(), FoundActors);
		if (FoundActors.Num() > 0)
		{
			EnemyTemporary = Cast<ASevenCharacter>(FoundActors[0]);
		}
	}
}

void ASevenCharacter::AttackStart()
{
	TArray<ASevenCharacter*> FoundActors = { EnemyTemporary };
	
	// Find closest enemy (to whose is attack meant)
	for (auto& Enemy : FoundActors)
	{
		float DotProduct = FVector::DotProduct(GetActorForwardVector().GetSafeNormal(), EnemyTemporary->GetActorLocation().GetSafeNormal());
		float Angle = FMath::Acos(DotProduct); // TODO DELETE
		UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter] AttackStart.DotProduct %f %f"), DotProduct, Angle);

		if (DotProduct > 0.6 && DotProduct <= 1)
		{
			// Take closest one
			FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(GetActorForwardVector(), Enemy->GetActorLocation());
			RootComponent->SetWorldRotation(PlayerRot);
			break;
		}
	}
}
	
float ASevenCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter] TakeDamage"));
	return 0.0f;
}

void ASevenCharacter::Space(const FInputActionValue& Value)
{
	ComboComponent->UseCombo("xx");
	//Jump();
}

void ASevenCharacter::Evade(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("[ASevenCharacter] Evade"));
}

void ASevenCharacter::StopSpace(const FInputActionValue& Value)
{
	StopJumping();
}

void ASevenCharacter::Fire(const FInputActionValue& Value)
{
	// Attack
	AnimationComponent->Play(LightAttack, FName("1"));
}


void ASevenCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
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

void ASevenCharacter::PerformWeaponTrace()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->PerformTrace();
	}
}



