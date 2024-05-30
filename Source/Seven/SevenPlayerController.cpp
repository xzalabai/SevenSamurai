#include "SevenPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "SevenCharacter.h"
#include "AttributesComponent.h"
#include "ControllableInterface.h"
#include "Kismet\GameplayStatics.h"

void ASevenPlayerController::BeginPlay()
{
	UniqueIDCounter = 0;
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

	//Switch(FInputActionValue{});
	// TODO: INITIAL Posses (should be moved to function!!
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
					break;
				}

			}
		}
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASevenCharacter::StaticClass(), FoundActors);
	OnUpdateStatus.AddUObject(this, &ASevenPlayerController::OnCharacterKilled);
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
		EnhancedInputComponent->BindAction(ToggleMovementAction, ETriggerEvent::Completed, this, &ASevenPlayerController::ToggleMovement);
		EnhancedInputComponent->BindAction(FireRMBAction, ETriggerEvent::Started, this, &ASevenPlayerController::FireRMB, ETriggerEvent::Started);
		EnhancedInputComponent->BindAction(FireRMBAction, ETriggerEvent::Completed, this, &ASevenPlayerController::FireRMB, ETriggerEvent::Completed);
		EnhancedInputComponent->BindAction(FireRMBAction, ETriggerEvent::Canceled, this, &ASevenPlayerController::FireRMB, ETriggerEvent::Canceled);

		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &ASevenPlayerController::BlockStart);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &ASevenPlayerController::BlockEnd);
		
		EnhancedInputComponent->BindAction(SwitchAction, ETriggerEvent::Completed, this, &ASevenPlayerController::Switch);
		
		EnhancedInputComponent->BindAction(Special1Action, ETriggerEvent::Completed, this, &ASevenPlayerController::Special, (int8)1);
		EnhancedInputComponent->BindAction(Special2Action, ETriggerEvent::Completed, this, &ASevenPlayerController::Special, (int8)2);
		EnhancedInputComponent->BindAction(Special3Action, ETriggerEvent::Completed, this, &ASevenPlayerController::Special, (int8)3);
		
		EnhancedInputComponent->BindAction(EvadeAction, ETriggerEvent::Triggered, this, &ASevenPlayerController::Evade);
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

void ASevenPlayerController::ToggleMovement(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("[ASevenPlayerController] ToggleMovement"));
	if (IControllableInterface* ControlledEntity = CastChecked<IControllableInterface>(GetControlledActor()))
	{
		ControlledEntity->ToggleMovement(Value);
	}
}

void ASevenPlayerController::Look(const FInputActionValue& Value)
{
	if (IControllableInterface* ControlledEntity = CastChecked<IControllableInterface>(GetControlledActor()))
	{
		ControlledEntity->Look(Value);
	}
}

void ASevenPlayerController::FireRMB(const FInputActionValue& Value, const ETriggerEvent TriggerEvent)
{
	if (IControllableInterface* ControlledEntity = CastChecked<IControllableInterface>(GetControlledActor()))
	{
		ControlledEntity->FireRMB(TriggerEvent);
	}
}

void ASevenPlayerController::Switch(const FInputActionValue& Value)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASevenCharacter::StaticClass(), FoundActors);
	for (const ASevenCharacter* SevenCharacter : SevenCharacters)
	{
		if (IsValid(SevenCharacter))
		{
			if (SevenCharacter != GetPossessedCharacter() && SevenCharactersStatus[SevenCharacter->GetUniqueID()] != EEnemyStatus::Dead)
			{
				SwitchSevenCharacter(SevenCharacter);
				break;
			}
		}
	}
	//if (bGodView)
	//{
	//	TArray<AActor*> FoundActors;
	//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASevenCharacter::StaticClass(), FoundActors);
	//	if (FoundActors.Num() > 0)
	//	{
	//		for (AActor* Act : FoundActors)
	//		{
	//			if (ASevenCharacter* Char = Cast<ASevenCharacter>(Act))
	//			{
	//				if (Char->CanBePossessed())
	//				{
	//					GodView->Enable(false);
	//					UE_LOG(LogTemp, Warning, TEXT("[ASevenPlayerController] Switch() Posses Player"));
	//					Possess(Char);
	//					SetViewTargetWithBlend(Char);
	//					bGodView = false;
	//				}
	//				
	//			}
	//		}
	//		
	//	}
	//}
	//else
	//{
	//	UnPossess();
	//	// TODO Cache
	//	TArray<AActor*> FoundActors;
	//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASevenCharacter::StaticClass(), FoundActors);
	//	if (FoundActors.Num() > 0)
	//	{
	//		if (ASevenCharacter* Char = Cast<ASevenCharacter>(FoundActors[0]))
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("[ASevenPlayerController] Switch() Posses Player"));
	//			SetViewTargetWithBlend(GodView);
	//			GodView->Enable(true);
	//		}
	//	}		
	//	bGodView = true;
	//}
}

void ASevenPlayerController::Evade(const FInputActionValue& Value)
{
	if (IControllableInterface* ControlledEntity = CastChecked<IControllableInterface>(GetControlledActor()))
	{
		ControlledEntity->Evade(Value);
	}
}

void ASevenPlayerController::Special(const FInputActionValue& Value, const int8 Number)
{
	if (IControllableInterface* ControlledEntity = CastChecked<IControllableInterface>(GetControlledActor()))
	{
		ControlledEntity->Special(Number);
	}
}

void ASevenPlayerController::BlockStart(const FInputActionValue& Value)
{
	if (IControllableInterface* ControlledEntity = CastChecked<IControllableInterface>(GetControlledActor()))
	{
		ControlledEntity->Block(true);
	}
}

void ASevenPlayerController::BlockEnd(const FInputActionValue& Value)
{
	if (IControllableInterface* ControlledEntity = CastChecked<IControllableInterface>(GetControlledActor()))
	{
		ControlledEntity->Block(false);
	}
}

TArray<const ASevenCharacter*> ASevenPlayerController::GetAIControlledAllies()
{
	TArray<const ASevenCharacter*> AIControlledAllies;
	ASevenCharacter* PlayerControlledAlly = GetPossessedCharacter();

	for (const ASevenCharacter* Seven : SevenCharacters)
	{
		if (Seven == PlayerControlledAlly || !Seven->IsAlive())
		{
			continue;
		}
		AIControlledAllies.Add(Seven);
	}

	return AIControlledAllies;
}

const TArray<const ASevenCharacter*> ASevenPlayerController::GetSevenCharacters()
{
	return SevenCharacters;
}

void ASevenPlayerController::UpdateStatus(const AActor* Actor, const EEnemyStatus Status)
{
	// This should be removed to something like SevenGameMode
	const ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(Actor);
	const int8 CharacterID = SevenCharacter->GetUniqueID();
	//UE_LOG(LogTemp, Warning, TEXT("[ASevenPlayerController] UpdateStatus: %d"), CharacterID);
	
	if (Status == EEnemyStatus::IncomingAttack)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ASevenPlayerController] UpdateStatus.IncomingAttack"));
	}

	if (SevenCharacter && SevenCharacter->IsEnemy())
	{
		// Updating list of enemies
		if (!EnemiesStatus.Contains(CharacterID))
		{
			EnemiesStatus.Add({ CharacterID, Status });
			Enemies.Add(SevenCharacter);
		}
		else
		{
			EnemiesStatus[CharacterID] = Status;
		}
	}
	else
	{
		// Updating list of SevenCharactersStatus (playable characters)
		if (!SevenCharactersStatus.Contains(CharacterID))
		{
			SevenCharactersStatus.Add({ CharacterID, Status });
			SevenCharacters.Add(SevenCharacter);
		}
		else
		{
			SevenCharactersStatus[CharacterID] = Status;
		}
	}
	
	OnUpdateStatus.Broadcast(Actor, Status);

	if (SevenCharacter == GetPossessedCharacter() && Status == EEnemyStatus::Dead)
	{
		Switch(FInputActionValue());
	}
}

const EEnemyStatus ASevenPlayerController::GetEnemyStatus(const int8 CharacterID) const
{
	if (!EnemiesStatus.Contains(CharacterID))
	{
		UE_LOG(LogTemp, Error, TEXT("[ASevenPlayerController] GetEnemyStatus Non Existing Character ID"));
		return EEnemyStatus::None;
	}
	return EnemiesStatus[CharacterID];
}

bool ASevenPlayerController::HasAnyEnemyStatus(const EEnemyStatus& Status) const
{
	for (auto &Enemy : EnemiesStatus)
	{
		if (Enemy.Value == Status)
		{
			return true;
		}
	}
	return false;
}

void ASevenPlayerController::OnCharacterKilled(const AActor* Actor, const EEnemyStatus Status)
{
	const ASevenCharacter* KilledCharacter = Cast<ASevenCharacter>(Actor);
	if (Status == EEnemyStatus::Dead && KilledCharacter->IsEnemy() && KilledCharacter != GetPossessedCharacter())
	{
		if (ASevenCharacter* SevenCharacter = GetPossessedCharacter())
		{
			SevenCharacter->AC_Attribute->Add(EItemType::XP, 10);
		}
	}
}

ASevenCharacter* ASevenPlayerController::GetPossessedCharacter()
{
	ASevenCharacter* PossessedCharacter = GetPawn<ASevenCharacter>();
	return PossessedCharacter;
}

ASevenCharacter* ASevenPlayerController::GetAnyAliveEnemy()
{
	for (const ASevenCharacter* Enemy : Enemies)
	{
		if (Enemy->IsAlive())
		{
			return const_cast<ASevenCharacter*>(Enemy);
		}
	}
	UE_LOG(LogTemp, Error, TEXT("[ASevenPlayerController].GetAnyAliveEnemy No alive enemies left."));
	return nullptr;
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





