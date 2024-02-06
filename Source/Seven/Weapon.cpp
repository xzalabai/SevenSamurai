#include "Weapon.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

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

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::AttachToSocket(USkeletalMeshComponent* PlayerMesh, FName SocketName)
{
	AttachToComponent(PlayerMesh, FAttachmentTransformRules::KeepWorldTransform, "hand_rSocket");
	FTransform SocketTransform = PlayerMesh->GetSocketTransform(SocketName);
	SetActorTransform(SocketTransform);
}

void AWeapon::PerformTrace()
{
	FHitResult OutHit;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetAttachParentActor());
	bool bHit = UKismetSystemLibrary::BoxTraceSingle(
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

	//if (ICharacterInterface* ITarget = Cast<ICharacterInterface>(OutHit.GetActor()))
	//{
	//	if (AffectedActors.Contains(OutHit.GetActor()))
	//	{
	//		UE_LOG(LogTemp, Display, TEXT("[AWeapon] AffectedActors"));
	//		// Actor is already in the list of affected
	//		return;
	//	}
	//	ITarget->OnReceivedHit(OutHit.ImpactNormal, OutHit.Location, GetAttachParentActor(), Damage);
	//	AffectedActors.Add(OutHit.GetActor());
	//}
}

