#include "VV_EntityBase.h"
#include "PaperSpriteComponent.h"
#include "MV_PlayerController.h"
#include "Components/CapsuleComponent.h"
#include <Kismet\GameplayStatics.h>
#include "GameController.h"


AVV_EntityBase::AVV_EntityBase()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root Scene Component"));
	RenderComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("RenderComponent"));

	RootComponent = CapsuleComponent;

	RenderComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	RenderComponent->Mobility = EComponentMobility::Movable;
	RenderComponent->SetupAttachment(RootComponent);
}

void AVV_EntityBase::BeginPlay()
{
	Super::BeginPlay();
	MV_PlayerController = Cast<AMV_PlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
}

void AVV_EntityBase::OnOverlapAction()
{
	UE_LOG(LogTemp, Error, TEXT("[AVV_EntityBase] OnOverlapAction THIS SHOULD NOT BE TRIGGERED"));
}
