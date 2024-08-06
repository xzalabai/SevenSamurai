#include "Shield.h"
#include "Components/BoxComponent.h"
#include "Weapon.h"
#include "AttackComponent.h"
#include "SevenCharacter.h"


AShield::AShield()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	BlockCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("Block Collider Box Component"));
	SetRootComponent(BlockCollider);
	MeshComponent->SetupAttachment(BlockCollider);
}

void AShield::AttachToSocket(USkeletalMeshComponent* PlayerMesh, FName SocketName)
{
	AttachToComponent(PlayerMesh, FAttachmentTransformRules::KeepWorldTransform, SocketName);
	FTransform SocketTransform = PlayerMesh->GetSocketTransform(SocketName);
	SetActorTransform(SocketTransform);
	CachedSevenCharacter = Cast<ASevenCharacter>(GetAttachParentActor());
}

void AShield::EnableShieldHits(bool bEnable)
{
	BlockCollider->SetGenerateOverlapEvents(bEnable);
}

void AShield::BeginPlay()
{
	Super::BeginPlay();
	BlockCollider->OnComponentHit.AddUniqueDynamic(this, &AShield::OnHit);
	EnableShieldHits(false);
}

void AShield::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Error, TEXT("[AShield] HIT CHARACTER %s"), *OtherActor->GetName());
	if (ASevenCharacter* HitEnemy = Cast<ASevenCharacter>(OtherActor))
	{
		if (HitEnemy != CachedSevenCharacter && CachedSevenCharacter->GetAttackComponent()->GetLastUsedCombo())
		{
			CachedSevenCharacter->GetAttackComponent()->GetLastUsedCombo()->DealDamage(HitEnemy);
		}
	}
}
