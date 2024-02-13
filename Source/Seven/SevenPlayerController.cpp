#include "SevenPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "SevenCharacter.h"
#include "ControllableInterface.h"
#include "Kismet\GameplayStatics.h"

void ASevenPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Display, TEXT("[ASevenPlayerController] BeginPlay"));
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	GodView = Cast<AGodView>(UGameplayStatics::GetActorOfClass(this, AGodView::StaticClass()));
	if (!GodView)
	{
		UE_LOG(LogTemp, Error, TEXT("[ASevenPlayerController] GodView actor is not present!"));
	}

	Switch(FInputActionValue{});

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
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &ASevenPlayerController::Fire);
		EnhancedInputComponent->BindAction(SwitchAction, ETriggerEvent::Completed, this, &ASevenPlayerController::Switch);
		EnhancedInputComponent->BindAction(Special1Action, ETriggerEvent::Completed, this, &ASevenPlayerController::Special1);
	}
}

void ASevenPlayerController::Space(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("[ASevenPlayerController] Space"));
	if (IControllableInterface* ControlledEntity = CastChecked<IControllableInterface>(GetControlledActor()))
	{
		ControlledEntity->Space(Value);
	}
}

void ASevenPlayerController::StopSpace(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("[ASevenPlayerController] StopSpace"));
	if (IControllableInterface* ControlledEntity = CastChecked<IControllableInterface>(GetControlledActor()))
	{
		ControlledEntity->StopSpace(Value);
	}
}

void ASevenPlayerController::Move(const FInputActionValue& Value)
{
	if (IControllableInterface* ControlledEntity = CastChecked<IControllableInterface>(GetControlledActor()))
	{
		ControlledEntity->Move(Value);
	}
}

void ASevenPlayerController::Fire(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("[ASevenPlayerController] Fire"));
	if (IControllableInterface* ControlledEntity = CastChecked<IControllableInterface>(GetControlledActor()))
	{
		ControlledEntity->Fire(Value);
	}
}

void ASevenPlayerController::Look(const FInputActionValue& Value)
{
	if (IControllableInterface* ControlledEntity = CastChecked<IControllableInterface>(GetControlledActor()))
	{
		ControlledEntity->Look(Value);
	}
}

void ASevenPlayerController::Switch(const FInputActionValue& Value)
{
	if (bGodView)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASevenCharacter::StaticClass(), FoundActors);
		if (FoundActors.Num() > 0)
		{
			for (AActor* Act : FoundActors)
			{
				if (ASevenCharacter* Char = Cast<ASevenCharacter>(Act))
				{
					if (Char->CanBePossessed())
					{
						GodView->Enable(false);
						UE_LOG(LogTemp, Warning, TEXT("[ASevenPlayerController] Switch() Posses Player"));
						Possess(Char);
						SetViewTargetWithBlend(Char);
						bGodView = false;
					}
					
				}
			}
			
		}
	}
	else
	{
		UnPossess();
		// TODO Cache
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASevenCharacter::StaticClass(), FoundActors);
		if (FoundActors.Num() > 0)
		{
			if (ASevenCharacter* Char = Cast<ASevenCharacter>(FoundActors[0]))
			{
				UE_LOG(LogTemp, Warning, TEXT("[ASevenPlayerController] Switch() Posses Player"));
				SetViewTargetWithBlend(GodView);
				GodView->Enable(true);
			}
		}		
		bGodView = true;
	}
}

void ASevenPlayerController::Evade(const FInputActionValue& Value)
{
	if (IControllableInterface* ControlledEntity = CastChecked<IControllableInterface>(GetControlledActor()))
	{
		ControlledEntity->Evade(Value);
	}
}

void ASevenPlayerController::Special1(const FInputActionValue& Value)
{
	if (IControllableInterface* ControlledEntity = CastChecked<IControllableInterface>(GetControlledActor()))
	{
		ControlledEntity->Special(1);
	}
}

TObjectPtr<AActor> ASevenPlayerController::GetControlledActor()
{
	// returns either GodView or GetPawn()
	if (GetPawn())
	{
		return GetPawn();
	}
	return GodView ? GodView : nullptr;
}

