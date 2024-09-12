#include "RollDice.h"
#include "SevenPlayerController.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include <Kismet\GameplayStatics.h>
#include "SevenGameMode.h"

ARollDice::ARollDice()
{
	PrimaryActorTick.bCanEverTick = false;
	Plane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Plane"));
	Dice = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Dice"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	
	UpperBound = CreateDefaultSubobject<UBoxComponent>(TEXT("UpperBound"));
	LowerBound = CreateDefaultSubobject<UBoxComponent>(TEXT("LowerBound"));

	SetRootComponent(Plane);

	Dice->SetupAttachment(RootComponent);
	Camera->SetupAttachment(RootComponent);

	UpperBound->SetupAttachment(Dice);
	LowerBound->SetupAttachment(Dice);
}

void ARollDice::BeginPlay()
{
	Super::BeginPlay();
	
	Plane->OnComponentBeginOverlap.AddDynamic(this, &ARollDice::OnComponentBeginOverlap);
	//LowerBound->OnComponentBeginOverlap.AddDynamic(this, &ARollDice::OnComponentBeginOverlap);

}

void ARollDice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARollDice::Activate()
{
	ASevenPlayerController* SevenPlayerController = Cast<ASevenPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	SevenPlayerController->UnPossess();
	SevenPlayerController->SetViewTarget(this);
	Camera->Activate();

	Plane->SetGenerateOverlapEvents(true);	
	UpperBound->SetGenerateOverlapEvents(true);
	LowerBound->SetGenerateOverlapEvents(true);

	Roll();
}

void ARollDice::Roll()
{
	Dice->SetSimulatePhysics(true);
	// Toss upwards
	const int RandomImpulse = FMath::RandRange(200, 400);
	Dice->AddImpulse(FVector(0.0f, 0.0f, RandomImpulse), NAME_None, true);

	// Add rotation
	const FVector RotationAxis = FVector(FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f), FMath::RandRange(-1.0f, 1.0f));  // Random axis
	const int RandomStrength = FMath::RandRange(500, 800);

	Dice->AddAngularImpulseInDegrees(RotationAxis * RandomStrength, NAME_None, true);
}

void ARollDice::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (UBoxComponent* BoxComponent = Cast<UBoxComponent>(OtherComp))
	{
		bSuccessfulRoll = (BoxComponent == UpperBound) ? true : false;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ARollDice::ShowResultOfRoll, 2, false, -1);
	}	
}

void ARollDice::ShowResultOfRoll()
{
	ASevenGameMode* SevenGameMode = Cast<ASevenGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	SevenGameMode->OnRolledDice(bSuccessfulRoll);
}

