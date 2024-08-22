#include "Shield.h"
#include "Components/BoxComponent.h"
#include "Weapon.h"
#include "AttackComponent.h"
#include "AnimationComponent.h"
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
	BlockCollider->SetUseCCD(bEnable);
	BlockCollider->SetGenerateOverlapEvents(bEnable);
}

void AShield::BeginPlay()
{
	Super::BeginPlay();
	BlockCollider->OnComponentBeginOverlap.AddDynamic(this, &AShield::OnOverlapBegin);
	EnableShieldHits(false);
}

void AShield::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ASevenCharacter* HitEnemy = Cast<ASevenCharacter>(OtherActor)) // TODO: Consider insteadd of this having tags..
	{
		if (HitEnemy != CachedSevenCharacter && CachedSevenCharacter->GetAttackComponent()->GetLastUsedCombo())
		{
			if (CachedSevenCharacter->AC_Animation->GetCurrentMontageType() != EMontageType::HitReaction)
			{
				UE_LOG(LogTemp, Error, TEXT("[ASevenCharacter] AttackWaxxxxxxxxxxxxxxxxsParried %d"), (int) CachedSevenCharacter->AC_Animation->GetCurrentMontageType());
				CachedSevenCharacter->ReceivedHit(CachedSevenCharacter->GetAttackInfo());

			}
				
		}
	}
}
