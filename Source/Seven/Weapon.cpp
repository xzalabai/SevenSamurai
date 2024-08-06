#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PublicEnums.h"
#include "AttackComponent.h"
#include "SevenCharacter.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	ParticleSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle System"));
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	SetRootComponent(RootSceneComponent);
	MeshComponent->SetupAttachment(RootSceneComponent);
	ParticleSystem->SetupAttachment(RootComponent);

	StartTrace = CreateDefaultSubobject<USceneComponent>(TEXT("Start Trace"));
	StartTrace->SetupAttachment(RootComponent);

	EndTrace = CreateDefaultSubobject<USceneComponent>(TEXT("End Trace"));
	EndTrace->SetupAttachment(RootComponent);
}

void AWeapon::AttachToSocket(USkeletalMeshComponent* PlayerMesh, FName SocketName)
{
	AttachToComponent(PlayerMesh, FAttachmentTransformRules::KeepWorldTransform, SocketName);
	FTransform SocketTransform = PlayerMesh->GetSocketTransform(SocketName);
	SetActorTransform(SocketTransform);
	CachedSevenCharacter = Cast<ASevenCharacter>(GetAttachParentActor());
}

void AWeapon::PerformTrace()
{
	bool bHit = UKismetSystemLibrary::BoxTraceMulti(
		this,
		StartTrace->GetComponentLocation(),
		EndTrace->GetComponentLocation(),
		FVector(10.0f, 10.0f, 10.0f),
		StartTrace->GetComponentRotation(),
		UEngineTypes::ConvertToTraceType(ECC_WorldDynamic),
		false,
		ActorsToIgnore, EDrawDebugTrace::None,
		OutHit,
		true);

	if (bHit)
	{
		for (FHitResult& Hit : OutHit)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActors.Contains(HitActor))
			{
				// Already listed as hit
				continue;
			}

			if (ASevenCharacter* Target = Cast<ASevenCharacter>(HitActor))
			{
				Target->ReceivedHit(AttackInfo);
				HitActors.Add(Hit.GetActor());
				//UE_LOG(LogTemp, Display, TEXT("[AWeapon] PerformTrace.Hit:%s"), *Hit.GetActor()->GetName());
			}
		}
		
	}
}

void AWeapon::AttackStart()
{
	HitActors.Empty();
	OutHit.Empty();
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetAttachParentActor());

	TObjectPtr<ASevenCharacter> SevenCharacter = Cast<ASevenCharacter>(GetAttachParentActor());

	AttackInfo = SevenCharacter->GetAttackComponent()->GetAttackInfo();
}

