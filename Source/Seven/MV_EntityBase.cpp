#include "MV_EntityBase.h"
#include "MissionDA.h"
#include "PaperSpriteComponent.h"
#include "Components/CapsuleComponent.h"
#include <Kismet\GameplayStatics.h>
#include "GameController.h"

void AMV_EntityBase::BeginPlay()
{
	Super::BeginPlay();
	if (MissionDA && MissionDA->MissionStatus == EStatus::Completed && MissionDA->MissionCompleteImage)
	{
		UpdateImage(MissionDA->MissionCompleteImage);
	}
	else if (MissionDA)
	{
		UpdateImage(MissionDA->Image);
	}
}

AMV_EntityBase::AMV_EntityBase()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root Scene Component"));
	RenderComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("RenderComponent"));

	RootComponent = CapsuleComponent;

	RenderComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	RenderComponent->Mobility = EComponentMobility::Movable;
	RenderComponent->SetupAttachment(RootComponent);
}

UMissionDA* AMV_EntityBase::GetMissionDA() const
{
	return MissionDA;
}

void AMV_EntityBase::UpdateImage(UPaperSprite* NewSprite) const
{
	if (NewSprite)
	{
		RenderComponent->SetSprite(NewSprite);
	}
	else
	{
		RenderComponent->SetSprite(MissionDA->Image);
	}
	
}

void AMV_EntityBase::OnOverlapAction()
{
	if (MissionDA->MissionStatus != EStatus::Completed)
	{
		UGameController* GameController = Cast<UGameController>(Cast<UGameInstance>(GetWorld()->GetGameInstance())->GetSubsystem<UGameController>());
		GameController->SetStartedEntity(this, GetMissionDA());
	}
}

void AMV_EntityBase::OnPossessed()
{
	UE_LOG(LogTemp, Display, TEXT("[AMV_EntityBase] OnPossessed"));
}

