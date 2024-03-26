#include "Item.h"
#include "Components/SphereComponent.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	TriggerCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger Collider"));
	SetRootComponent(TriggerCollider);
	
	MeshComponent->SetupAttachment(TriggerCollider);
	TriggerCollider->SetupAttachment(TriggerCollider);
}	

void AItem::BeginPlay()
{
	Super::BeginPlay();	
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItem::Init(const EItemType NewItemType, const int NewAmount)
{
	ItemType = NewItemType;
	Amount = NewAmount;
}

