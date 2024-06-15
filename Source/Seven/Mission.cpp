#include "Mission.h"
#include "Components/SphereComponent.h"
#include <Kismet\GameplayStatics.h>
#include "SevenCharacter.h"
#include "EnemyCharacter.h"


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
	Area->OnComponentBeginOverlap.AddDynamic(this, &AMission::OnOverlapBegin);
	check(MissionType != EMissionType::NotProvided);
	if (bSideMission)
	{
		// this will be turned on for all
		ActivateMission(false);
	}
}

void AMission::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(OtherActor);
	if (SevenCharacter->IsEnemy() || !SevenCharacter->IsPlayerControlled())
	{
		return;
	}

	OnCharacterOverlappedMission.ExecuteIfBound(ID);
}

void AMission::MissionComplete(bool bWin) const
{
	if (!bSideMission && SideMission)
	{
		SideMission->ActivateMission(true);
	}
}

void AMission::ActivateMission(bool bEnable)
{
	UE_LOG(LogTemp, Display, TEXT("[AMission].ActivateMission %d"), ID);
	Area->SetGenerateOverlapEvents(bEnable);
}

void AMission::MissionStarted() const
{
	Area->SetGenerateOverlapEvents(false);

	//FActorSpawnParameters SpawnParams;
	//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (const TPair<int32, TSubclassOf<AEnemyCharacter>>& Pair : EnemiesToSpawn)
	{
		for (int i = 0; i < Pair.Key; i++)
		{
			const FTransform T(FRotator(), EnemySpawns[i % EnemySpawns.Num()]->GetComponentLocation());
			AEnemyCharacter* Enemy = GetWorld()->SpawnActorDeferred<AEnemyCharacter>(Pair.Value, T, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			Enemy->MissionType = MissionType;
			// Set AI
			Enemy->FinishSpawning(T);
		}
	}
}

