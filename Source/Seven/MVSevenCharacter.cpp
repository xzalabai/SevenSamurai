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
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetActorTickInterval(2.0f);
	
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
	Camp(bIsCamp);
	MovementComponent->MaxSpeed = Stamina * 10;
}


void AMVSevenCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsCamp)
	{
		AccumulatedHPGained += 5;
	}
	else if (bIsMoving)
	{
		Stamina = FMath::Max(Stamina - 5, 10);
		MovementComponent->MaxSpeed = Stamina * 10;
	}
	UE_LOG(LogTemp, Display, TEXT("[AMVSevenCharacter] Tick, HP Gained: %d, Stamina: %d"), AccumulatedHPGained, Stamina);
}

void AMVSevenCharacter::UpdateSevenCharactersHP()
{
	UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
	GameController->UpdateSevenCharactersHP(AccumulatedHPGained);
	AccumulatedHPGained = 0;
}

void AMVSevenCharacter::Camp(bool bEnable)
{
	if (bEnable)
	{
		UpdateImage(CampImage);
		bIsMoving = false;
	}
	else
	{
		UpdateImage(CharacterImage);
		UpdateSevenCharactersHP();
	}
	bIsCamp = bEnable;
	OnCamp(bEnable);
}

void AMVSevenCharacter::UpdateImage(UPaperSprite* NewSprite) const
{
	RenderComponent->SetSprite(NewSprite);
}

void AMVSevenCharacter::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Error, TEXT("[AMVSevenCharacter] OnComponentBeginOverlap %s, %d"), *OtherActor->GetName(), bFromSweep ? 1 :0);

	UpdateSevenCharactersHP();

	if (IMV_OverlapInterface* OtherEntity = Cast<IMV_OverlapInterface>(OtherActor))
	{
		OtherEntity->OnOverlapAction();
	}
}

void AMVSevenCharacter::SetCurrentAreaID(const int8 NewAreaID)
{
	CurrentAreaID = NewAreaID;
}

