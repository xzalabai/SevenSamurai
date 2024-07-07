#include "MVSevenCharacter.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "PaperSpriteComponent.h"
#include "Components/CapsuleComponent.h"
#include "MV_EntityBase.h"
#include "MissionDA.h"
#include "Mission.h"
#include <Kismet\GameplayStatics.h>
#include "GameController.h"

AMVSevenCharacter::AMVSevenCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root Scene Component"));
	RenderComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("RenderComponent"));
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Movement Component"));

	RootComponent = CapsuleComponent;

	RenderComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	RenderComponent->Mobility = EComponentMobility::Movable;
	RenderComponent->SetupAttachment(RootComponent);
}

void AMVSevenCharacter::BeginPlay()
{
	Super::BeginPlay();
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AMVSevenCharacter::OnComponentBeginOverlap);
}

void AMVSevenCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMVSevenCharacter::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Display, TEXT("[AMVSevenCharacter] OnComponentBeginOverlap %s"), *OtherActor->GetName());

	if (IMV_OverlapInterface* OtherEntity = Cast<IMV_OverlapInterface>(OtherActor))
	{
		OtherEntity->OnOverlapAction();
	}
}

void AMVSevenCharacter::SetCurrentAreaID(const int8 NewAreaID)
{
	CurrentAreaID = NewAreaID;
}

