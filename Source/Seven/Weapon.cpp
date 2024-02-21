#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/Character.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PublicEnums.h"
#include "SevenCharacter.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	//TriggerCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger Collider"));
	ParticleSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle System"));
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	SetRootComponent(RootSceneComponent);
	MeshComponent->SetupAttachment(RootSceneComponent);
	//TriggerCollider->SetupAttachment(MeshComponent);
	ParticleSystem->SetupAttachment(RootComponent);

	StartTrace = CreateDefaultSubobject<USceneComponent>(TEXT("Start Trace"));
	StartTrace->SetupAttachment(RootComponent);

	EndTrace = CreateDefaultSubobject<USceneComponent>(TEXT("End Trace"));
	EndTrace->SetupAttachment(RootComponent);
}

void AWeapon::AttachToSocket(USkeletalMeshComponent* PlayerMesh, FName SocketName)
{
	AttachToComponent(PlayerMesh, FAttachmentTransformRules::KeepWorldTransform, "hand_rSocket");
	FTransform SocketTransform = PlayerMesh->GetSocketTransform(SocketName);
	SetActorTransform(SocketTransform);
}

void AWeapon::ClearHitActors()
{
	HitActors.Empty();
}

void AWeapon::PerformTrace()
{
	TArray<FHitResult> OutHit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetAttachParentActor());
	bool bHit = UKismetSystemLibrary::BoxTraceMulti(
		this,
		StartTrace->GetComponentLocation(),
		EndTrace->GetComponentLocation(),
		FVector(3.3f, 3.3f, 3.3f),
		StartTrace->GetComponentRotation(),
		UEngineTypes::ConvertToTraceType(ECC_WorldDynamic),
		false,
		ActorsToIgnore, EDrawDebugTrace::Persistent,
		OutHit,
		true);
	//UE_LOG(LogTemp, Display, TEXT("[AWeapon] PerformTrace.bHit:%d"), bHit ? 1 : 0);
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
				Target->ReceivedHit(FAttackInfo{ static_cast<EAttackType>(0), 4, 10 });
				HitActors.Add(Hit.GetActor());
				UE_LOG(LogTemp, Display, TEXT("[AWeapon] PerformTrace.Hit:%s"), *Hit.GetActor()->GetName());
			}
		}
		
	}
}

