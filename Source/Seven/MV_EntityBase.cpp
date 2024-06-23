#include "MV_EntityBase.h"
#include "MissionDA.h"
#include "PaperSpriteComponent.h"
#include "Components/CapsuleComponent.h"
#include <Kismet\GameplayStatics.h>

void AMV_EntityBase::BeginPlay()
{
	Super::BeginPlay();
	if (MissionDA)
	{
		RenderComponent->SetSprite(MissionDA->Image);
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

const UMissionDA* AMV_EntityBase::GetMissionDA() const
{
	return MissionDA;
}

