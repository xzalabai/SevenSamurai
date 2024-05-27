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
	EnemySpawn = CreateDefaultSubobject<USceneComponent>(TEXT("Enemy Spawn"));
	SevenCharacterPosition = CreateDefaultSubobject<USceneComponent>(TEXT("SevenCharacterPosition"));


	Area->SetupAttachment(RootComponent);
	EnemySpawn->SetupAttachment(RootComponent);
	SevenCharacterPosition->SetupAttachment(RootComponent);

}

void AMission::BeginPlay()
{
	Super::BeginPlay();
	Area->OnComponentBeginOverlap.AddDynamic(this, &AMission::OnOverlapBegin);
}

void AMission::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMission::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Error, TEXT("[AMission].OnOverlapBegin "));
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(OtherActor);
	if (SevenCharacter->IsNPC() || !SevenCharacter->IsPlayerControlled())
	{
		return;
	}

	OnCharacterOverlappedMission.ExecuteIfBound(ID);
}

