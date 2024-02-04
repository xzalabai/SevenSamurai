#include "SevenPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "SevenCharacter.h"
#include "ControllableInterface.h"
#include "Kismet\GameplayStatics.h"

void ASevenPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	SetupInputComponent();
}

void ASevenPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(SpaceAction, ETriggerEvent::Triggered, this, &ASevenPlayerController::Space);
		EnhancedInputComponent->BindAction(SpaceAction, ETriggerEvent::Completed, this, &ASevenPlayerController::StopSpace);

		//Moving
		EnhancedInputComponent->BindAction(WSADAction, ETriggerEvent::Triggered, this, &ASevenPlayerController::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASevenPlayerController::Look);
		//Action
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &ASevenPlayerController::Fire);
		EnhancedInputComponent->BindAction(SwitchAction, ETriggerEvent::Completed, this, &ASevenPlayerController::Switch);
	}
}

void ASevenPlayerController::Space(const FInputActionValue& Value)
{
	if (!GetPawn())
	{
		return;
	}

	if (IControllableInterface* ControlledPawn = CastChecked<IControllableInterface>(GetPawn()))
	{
		ControlledPawn->Space(Value);
	}
}

void ASevenPlayerController::StopSpace(const FInputActionValue& Value)
{
	if (!GetPawn())
	{
		return;
	}

	if (IControllableInterface* ControlledPawn = CastChecked<IControllableInterface>(GetPawn()))
	{
		ControlledPawn->StopSpace(Value);
	}
}

void ASevenPlayerController::Move(const FInputActionValue& Value)
{
	if (!GetPawn())
	{
		return;
	}

	if (IControllableInterface* ControlledPawn = CastChecked<IControllableInterface>(GetPawn()))
	{
		ControlledPawn->Move(Value);
	}
}

void ASevenPlayerController::Fire(const FInputActionValue& Value)
{
	if (!GetPawn())
	{
		return;
	}
	if (IControllableInterface* ControlledPawn = Cast<IControllableInterface>(GetPawn()))
	{
		ControlledPawn->Fire(Value);
	}
}

void ASevenPlayerController::Look(const FInputActionValue& Value)
{
	if (!GetPawn())
	{
		return;
	}
	if (IControllableInterface* ControlledPawn = CastChecked<IControllableInterface>(GetPawn()))
	{
		ControlledPawn->Look(Value);
	}
}

void ASevenPlayerController::Switch(const FInputActionValue& Value)
{
	if (bBirdView)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASevenCharacter::StaticClass(), FoundActors);
		if (FoundActors.Num() > 0)
		{
			if (ASevenCharacter* Char = Cast<ASevenCharacter>(FoundActors[0]))
			{
				UE_LOG(LogTemp, Warning, TEXT("Posses"));
				Possess(Char);
				return;
			}
		}
	}
}

APawn* ASevenPlayerController::TryGetPawn()
{
	APawn* PossesedPawn = GetPawn();
	if (PossesedPawn != nullptr)
	{
		return PossesedPawn;
	}
	return nullptr;
}

