#include "Item.h"
#include "SevenCharacter.h"
#include "AttributesComponent.h"
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
	TriggerCollider->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnComponentBeginOverlap);
}

void AItem::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Use(OtherActor);
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

void AItem::Use(AActor* OtherActor)
{
	ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(OtherActor);
	if (!SevenCharacter)
	{
		return;
	}
	SevenCharacter->AC_Attribute->Add(ItemType, Amount);
	TriggerCollider->OnComponentBeginOverlap.RemoveDynamic(this, &AItem::OnComponentBeginOverlap);
	Destroy();
}

