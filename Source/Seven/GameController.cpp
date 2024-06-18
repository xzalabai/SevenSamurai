#include "GameController.h"
#include "SevenCharacter.h"
#include "SevenPlayerController.h"
#include "AttributesComponent.h"
#include "Kismet\GameplayStatics.h"

void UGameController::Initialize(FSubsystemCollectionBase& Collection)
{
	UniqueIDCounter = 0;
	OnStatusUpdate.AddUObject(this, &UGameController::OnCharacterKilled);

}

const EEnemyStatus UGameController::GetEnemyStatus(const int8 CharacterID) const
{
	return EEnemyStatus();
}

bool UGameController::HasAnyEnemyStatus(const EEnemyStatus& Status) const
{
	for (auto& Enemy : EnemiesStatus)
	{
		if (Enemy.Value == Status)
		{
			return true;
		}
	}
	return false;
}

void UGameController::OnCharacterKilled(const AActor* Actor, const EEnemyStatus Status)
{
	const ASevenCharacter* KilledCharacter = Cast<ASevenCharacter>(Actor);
	if (Status == EEnemyStatus::Dead && KilledCharacter->IsEnemy() && KilledCharacter != GetPossessedCharacter())
	{
		if (ASevenCharacter* SevenCharacter = GetPossessedCharacter())
		{
			SevenCharacter->AC_Attribute->Add(EItemType::XP, 20);
		}
	}
}

ASevenCharacter* UGameController::GetAnyAliveEnemy()
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

ASevenCharacter* UGameController::GetPossessedCharacter() const
{
	const ASevenPlayerController* SevenPlayerController = GetSevenPlayerController();
	ASevenCharacter* PossessedCharacter = SevenPlayerController->GetPawn<ASevenCharacter>();
	return PossessedCharacter;
}

ASevenPlayerController* UGameController::GetSevenPlayerController() const
{
	ASevenPlayerController* SevenPlayerController = Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (SevenPlayerController)
	{
		return SevenPlayerController;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[UGameController].SevenPlayerController is NULLPTR"));
		return nullptr;
	}
}

TArray<const ASevenCharacter*> UGameController::GetAIControlledAllies() const
{
	TArray<const ASevenCharacter*> AIControlledAllies;
	const ASevenCharacter* PlayerControlledAlly = GetPossessedCharacter();

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

const TArray<const ASevenCharacter*> UGameController::GetSevenCharacters() const
{
	return SevenCharacters;
}

void UGameController::Restart()
{
	//OnRestart.Broadcast(); 

	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void UGameController::UpdateStatus(const AActor* Actor, const EEnemyStatus Status)
{
	// This should be removed to something like SevenGameMode
	const ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(Actor);
	const int8 CharacterID = SevenCharacter->GetUniqueID();
	UE_LOG(LogTemp, Warning, TEXT("[ASevenPlayerController] UpdateStatus: %d"), CharacterID);

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

	OnStatusUpdate.Broadcast(Actor, Status);

	if (SevenCharacter == GetPossessedCharacter() && Status == EEnemyStatus::Dead)
	{
		ASevenPlayerController* SevenPlayerController = Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		SevenPlayerController->Switch(FInputActionValue());
	}
}
