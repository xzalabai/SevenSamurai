#include "Shield.h"
#include "Components/SphereComponent.h"
#include "Weapon.h"
#include "SevenCharacter.h"


AShield::AShield()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	BlockCollider = CreateDefaultSubobject<USphereComponent>(TEXT("BlockCollider Component"));
	SetRootComponent(BlockCollider);
	MeshComponent->SetupAttachment(BlockCollider);
}

void AShield::AttachToSocket(USkeletalMeshComponent* PlayerMesh, FName SocketName)
{
	AttachToComponent(PlayerMesh, FAttachmentTransformRules::KeepWorldTransform, SocketName);
	FTransform SocketTransform = PlayerMesh->GetSocketTransform(SocketName);
	SetActorTransform(SocketTransform);
}

void AShield::BeginPlay()
{
	Super::BeginPlay();
}