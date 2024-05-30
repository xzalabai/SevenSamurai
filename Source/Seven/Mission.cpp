#include "Mission.h"
#include "Components/SphereComponent.h"
#include <Kismet\GameplayStatics.h>
#include "SevenCharacter.h"


AMission::AMission()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	SetRootComponent(RootSceneComponent);

	Area = CreateDefaultSubobject<USphereComponent>(TEXT("Area"));
	//EnemySpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Enemy Spawn"));
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
	check(EnemiesCount > 0);
}

void AMission::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Error, TEXT("[AMission].OnOverlapBegin "));
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(OtherActor);
	if (SevenCharacter->IsEnemy() || !SevenCharacter->IsPlayerControlled())
	{
		return;
	}

	OnCharacterOverlappedMission.ExecuteIfBound(ID);
}

