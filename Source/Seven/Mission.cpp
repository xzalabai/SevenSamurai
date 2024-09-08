#include "Mission.h"
#include "Components/SphereComponent.h"
#include <Kismet\GameplayStatics.h>
#include "SevenCharacter.h"
#include "EnemyCharacter.h"
#include "SevenCharacterDA.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SevenGameMode.h"
#include "GameController.h"
#include "AIController.h"


AMission::AMission()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	SetRootComponent(RootSceneComponent);

	Area = CreateDefaultSubobject<USphereComponent>(TEXT("Area"));
	SevenCharactersPosition = CreateDefaultSubobject<USceneComponent>(TEXT("SevenCharacterPosition"));
	
	for (int i = 0; i < 3; i++)
	{
		FString NameFString = "EnemySpawn" + FString::FromInt(i);
		FName Name = FName(*NameFString);
		EnemySpawns.Add(CreateDefaultSubobject<USceneComponent>(Name));
	}

	Area->SetupAttachment(RootComponent);
	SevenCharactersPosition->SetupAttachment(RootComponent);
}

void AMission::BeginPlay()
{
	Super::BeginPlay();

	CachedSevenGameMode = Cast<ASevenGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	
	CachedSevenGameMode->UpdateMissionParameters(this);
	CachedSevenGameMode->OnStatusUpdate.AddUObject(this, &AMission::OnStatusUpdate);

	Area->OnComponentBeginOverlap.AddDynamic(this, &AMission::OnOverlapBegin);

	ActivateMission(true);
	MissionStarted();
}

void AMission::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(OtherActor);
	if (SevenCharacter->IsEnemy() || !SevenCharacter->IsPlayerControlled())
	{
		return;
	}
	MissionStarted();
}

void AMission::ActivateMission(bool bEnable)
{
	UE_LOG(LogTemp, Display, TEXT("[AMission].ActivateMission %d"), ID);
	Area->SetGenerateOverlapEvents(bEnable);
}

void AMission::MissionStarted()
{
	Area->SetGenerateOverlapEvents(false);

	SpawnSevenCharacter();
	SpawnEnemies();

	MoveAlliesToPlace();
}

void AMission::SpawnEnemies() const
{
	for (const FEnemyToSpawn& EnemyToSpawn : MissionDA->EnemiesToSpawn)
	{
		check(EnemyToSpawn.EnemyLevelDA);
		check(EnemyToSpawn.SevenCharacterType != ESevenCharacterType::NotProvided);
		check(CachedSevenGameMode->EnemiesCharacterMapping.Contains(EnemyToSpawn.SevenCharacterType));

		const TSubclassOf<ASevenCharacter>& EnemyClass = CachedSevenGameMode->EnemiesCharacterMapping[EnemyToSpawn.SevenCharacterType];
		for (int i = 0; i < EnemyToSpawn.Amount; ++i)
		{
			const FTransform T(EnemySpawns[i % EnemySpawns.Num()]->GetComponentRotation(), EnemySpawns[i % EnemySpawns.Num()]->GetComponentLocation());
			AEnemyCharacter* const Enemy = GetWorld()->SpawnActorDeferred<AEnemyCharacter>(
				EnemyClass,
				T,
				nullptr,
				nullptr,
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
			);
			Enemy->MissionType = MissionDA->MissionType;
			Enemy->EnemyLevelDA = EnemyToSpawn.EnemyLevelDA;
			Enemy->FinishSpawning(T);
			++EnemyCount;
		}
	}
}

void AMission::SpawnSevenCharacter() const
{
	const UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	const TArray<USevenCharacterDA*>& SelectedCharacters = GameController->GetSelectedCharacters();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (USevenCharacterDA* SevenCharacterDA : SelectedCharacters)
	{
		check(SevenCharacterDA->HP > 0);
		FTransform T{ SevenCharactersPosition->GetComponentLocation() }; // Todo create something like Enemies
		ASevenCharacter* SevenCharacter = GetWorld()->SpawnActorDeferred<ASevenCharacter>(
			SevenCharacterDA->RepresentingClass,
			T,
			nullptr,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn
		);

		SevenCharacter->SevenCharacterDA = SevenCharacterDA;
		SevenCharacter->FinishSpawning(T);
	}
}

void AMission::MoveAlliesToPlace()
{
	UE_LOG(LogTemp, Warning, TEXT("[UMissions].MoveSevenCharactersToPlace AIAlly move to mission area."));
	const TArray<const ASevenCharacter*>& AIControlledAllies = CachedSevenGameMode->GetAIControlledAllies();

	for (const ASevenCharacter* AIAlly : AIControlledAllies)
	{
		AAIController* AIController = Cast<AAIController>(AIAlly->GetController());
		UBlackboardComponent* BlackBoardComponent = AIController->GetBlackboardComponent();
		BlackBoardComponent->SetValueAsBool(TEXT("bFollowPlayer"), false);
		BlackBoardComponent->SetValueAsObject(TEXT("PositionToGo"), SevenCharactersPosition);
	}
}

void AMission::OnStatusUpdate(const ASevenCharacter* SevenCharacter, const ECharacterState Status)
{
	if (Status != ECharacterState::Dead)
	{
		return;
	}
		
	if (SevenCharacter->IsEnemy())
	{
		++EnemyKilledCount;

		if (EnemyKilledCount == EnemyCount)
		{
			UE_LOG(LogTemp, Display, TEXT("[UMissions].OnMissionEnd WIN"));
			CachedSevenGameMode->MissionEnd(true);
		}

	}
	else
	{
		++SevenCharactersKilledCount;

		if (SevenCharactersKilledCount == SevenCharacterCount)
		{
			UE_LOG(LogTemp, Display, TEXT("[UMissions].OnMissionEnd LOST"));
			CachedSevenGameMode->MissionEnd(false);
		}
	}
}

