#include "SevenPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "SevenCharacter.h"
#include "AttributesComponent.h"
#include "Kismet\GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "SevenGameMode.h"

void ASevenPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
	Switch(FInputActionValue());
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
		EnhancedInputComponent->BindAction(LockTargetAction, ETriggerEvent::Completed, this, &ASevenPlayerController::LockTarget);
		EnhancedInputComponent->BindAction(FireRMBAction, ETriggerEvent::Started, this, &ASevenPlayerController::FireRMB, ETriggerEvent::Started);
		EnhancedInputComponent->BindAction(FireRMBAction, ETriggerEvent::Completed, this, &ASevenPlayerController::FireRMB, ETriggerEvent::Completed);

		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &ASevenPlayerController::Run, ETriggerEvent::Started);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &ASevenPlayerController::Run, ETriggerEvent::Completed);


		//EnhancedInputComponent->BindAction(FireRMBAction, ETriggerEvent::Canceled, this, &ASevenPlayerController::FireRMB, ETriggerEvent::Canceled);

		//EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &ASevenPlayerController::BlockStart);
		//EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &ASevenPlayerController::BlockEnd);
		
		EnhancedInputComponent->BindAction(SwitchAction, ETriggerEvent::Completed, this, &ASevenPlayerController::Switch);
		
		EnhancedInputComponent->BindAction(Special1Action, ETriggerEvent::Completed, this, &ASevenPlayerController::Special, (int8)1);
		EnhancedInputComponent->BindAction(Special2Action, ETriggerEvent::Completed, this, &ASevenPlayerController::Special, (int8)2);
		EnhancedInputComponent->BindAction(Special3Action, ETriggerEvent::Completed, this, &ASevenPlayerController::Special, (int8)3);
		
		EnhancedInputComponent->BindAction(EvadeAction, ETriggerEvent::Triggered, this, &ASevenPlayerController::Evade);
	}
}

void ASevenPlayerController::Space(const FInputActionValue& Value)
{
	ASevenCharacter* ControllerCharacter = Cast<ASevenCharacter>(GetPawn()); // TODO: For all classes, stop casting every tick!!! cache it!!!
	if (ControllerCharacter)
	{
		ControllerCharacter->Space(Value);
	}
}

void ASevenPlayerController::StopSpace(const FInputActionValue& Value)
{
	ASevenCharacter* ControllerCharacter = Cast<ASevenCharacter>(GetPawn());
	if (ControllerCharacter)
	{
		ControllerCharacter->StopSpace(Value);
	}
}

void ASevenPlayerController::Move(const FInputActionValue& Value)
{
	ASevenCharacter* ControllerCharacter = Cast<ASevenCharacter>(GetPawn());
	if (ControllerCharacter)
	{
		ControllerCharacter->Move(Value);
	}
}

void ASevenPlayerController::Run(const FInputActionValue& Value, const ETriggerEvent TriggerEvent)
{
	UE_LOG(LogTemp, Error, TEXT("[ASevenCharacter] Run %d"), (int) TriggerEvent);
	ASevenCharacter* ControllerCharacter = Cast<ASevenCharacter>(GetPawn());
	//ControllerCharacter->FireRMB(TriggerEvent);
	
	if (ControllerCharacter)
	{
		ControllerCharacter->Run(TriggerEvent == ETriggerEvent::Started ? true : false);
	}
}

void ASevenPlayerController::Fire(const FInputActionValue& Value)
{
	ASevenCharacter* ControllerCharacter = Cast<ASevenCharacter>(GetPawn());
	if (ControllerCharacter)
	{
		ControllerCharacter->Fire(Value);
	}
}

void ASevenPlayerController::LockTarget(const FInputActionValue& Value)
{
	bLockTarget = !bLockTarget;
	ASevenCharacter* ControllerCharacter = Cast<ASevenCharacter>(GetPawn());
	if (ControllerCharacter)
	{
		ControllerCharacter->LockTarget(bLockTarget);
	}
}

void ASevenPlayerController::Look(const FInputActionValue& Value)
{
	ASevenCharacter* ControllerCharacter = Cast<ASevenCharacter>(GetPawn());
	
	if (ControllerCharacter)
	{
		ControllerCharacter->Look(Value);
	}
}

void ASevenPlayerController::FireRMB(const FInputActionValue& Value, const ETriggerEvent TriggerEvent)
{
	ASevenCharacter* ControllerCharacter = Cast<ASevenCharacter>(GetPawn());
	if (ControllerCharacter)
	{
		ControllerCharacter->Block(TriggerEvent == ETriggerEvent::Started ? true : false);
	}
}

void ASevenPlayerController::Switch(const FInputActionValue& Value)
{	
	const ASevenGameMode* SevenGameMode = Cast<ASevenGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	const TArray<const ASevenCharacter*>& SevenCharacters = SevenGameMode->GetSevenCharacters();
	const TMap<int8, ECharacterState>& SevenCharactersStatus = SevenGameMode->GetSevenCharactersStatus();

	for (const ASevenCharacter* SevenCharacter : SevenCharacters)
	{
		if (IsValid(SevenCharacter))
		{
			if (SevenCharacter != GetPossessedCharacter() && SevenCharactersStatus[SevenCharacter->GetUniqueID()] != ECharacterState::Dead)
			{
				SwitchSevenCharacter(SevenCharacter);
				break;
			}
		}
	}
}

void ASevenPlayerController::Evade(const FInputActionValue& Value)
{
	ASevenCharacter* ControllerCharacter = Cast<ASevenCharacter>(GetPawn());
	if (ControllerCharacter)
	{
		ControllerCharacter->Evade(Value);
	}
}

void ASevenPlayerController::Special(const FInputActionValue& Value, const int8 Number)
{
	ASevenCharacter* ControllerCharacter = Cast<ASevenCharacter>(GetPawn());
	if (ControllerCharacter)
	{
		ControllerCharacter->Special(Number);
	}
}

void ASevenPlayerController::UpdateUI(const EItemType ItemType, const float NewHP)
{
	if (ItemType == EItemType::HP)
	{
		float ValueForUI = NewHP / 3;
		UE_LOG(LogTemp, Error, TEXT("[ASevenCharacter].UpdateHP: %f"), (float)ValueForUI);
		UpdateHPWidget(ValueForUI);
	}
	else if (ItemType == EItemType::XP)
	{
		float ValueForUI = NewHP / 3;
		UE_LOG(LogTemp, Error, TEXT("[ASevenCharacter].UpdateHP: %f"), (float)ValueForUI);
		UpdateXPWidget(ValueForUI);
	}
}

void ASevenPlayerController::UpdateUpdateCharactersUI(const FName& Name, bool bDead)
{
	if (bDead)
	{
		// Add ,,X" cross
		UpdateDeadCharacter(Name);
	}
	else
	{
		// Highlight Selected Character
		UpdateSelectedCharacter(Name);
	}

}

ASevenCharacter* ASevenPlayerController::GetPossessedCharacter()
{
	ASevenCharacter* PossessedCharacter = GetPawn<ASevenCharacter>();
	return PossessedCharacter;
}





