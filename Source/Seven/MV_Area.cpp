#include "MV_Area.h"
#include "Components/BoxComponent.h"
#include "MV_Map.h"
#include "MVSevenCharacter.h"
#include "GameController.h"

AMV_Area::AMV_Area()
{
	PrimaryActorTick.bCanEverTick = true;
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Area"));
	SetRootComponent(BoxComponent);
}

void AMV_Area::BeginPlay()
{
	Super::BeginPlay();
}

void AMV_Area::OnOverlapAction()
{
	UE_LOG(LogTemp, Display, TEXT("[AMV_Area].OnOverlapAction Player entered area: %d"), UniqueAreaID);
	check(Map);
	TObjectPtr<AMVSevenCharacter> MV_SevenCharacter = Map->GetMVSevenCharacter();
	MV_SevenCharacter->SetCurrentAreaID(UniqueAreaID);
}

