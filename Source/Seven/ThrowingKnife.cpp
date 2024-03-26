#include "ThrowingKnife.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "SevenCharacter.h"
#include "PublicEnums.h"
#include "GameFramework\ProjectileMovementComponent.h"
#include "Kismet\KismetMathLibrary.h"


AThrowingKnife::AThrowingKnife()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Component"));
	TriggerCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger Collider"));
	ParticleSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Particle System"));
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	SetRootComponent(TriggerCollider);

	MeshComponent->SetupAttachment(TriggerCollider);
	TriggerCollider->SetupAttachment(TriggerCollider);
	ParticleSystem->SetupAttachment(TriggerCollider);

	ProjectileMovementComponent->SetUpdatedComponent(TriggerCollider);


	ProjectileMovementComponent->InitialSpeed = 1500;
	ProjectileMovementComponent->ProjectileGravityScale = 0.0f;


}

void AThrowingKnife::BeginPlay()
{
	Super::BeginPlay();
	TriggerCollider->OnComponentHit.AddUniqueDynamic(this, &AThrowingKnife::OnHit);
	AttackInfo = FAttackInfo(EAttackType::Throw, -1, 5, GetOwner());
}

void AThrowingKnife::DestroyActor()
{
	Destroy();
}

void AThrowingKnife::FireInDirection(const FVector& ShootDirection)
{
	FVector StartLocation(0, 0, 0);
	FVector EndLocation(100, 0, 0);
	FColor DebugColor(255, 0, 0); // Red color
	FColor DebugColor2(0, 255, 0); // Red color
	float ArrowSize = 10.0f;
	float Thickness = 2.0f;
	float Duration = 5.0f;
	uint8 DepthPriority = 0; // Default depth priority
	float Length = -1.0f; // Length of the arrow shaft, -1 to use default
	DrawDebugPoint(GetWorld(), ShootDirection, ArrowSize, DebugColor2, true, Duration, DepthPriority);

	UE_LOG(LogTemp, Display, TEXT("[AThrowingKnife] FireInDirection"));

	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ShootDirection);
	SetActorRotation(Rotation);

	GetWorldTimerManager().SetTimer(TimerHandle, this, &AThrowingKnife::DestroyActor, 3, false, 3);
	ProjectileMovementComponent->Velocity = GetActorForwardVector() * ProjectileMovementComponent->InitialSpeed;
	//Alternatively: ProjectileMovementComponent->Velocity = Rotation.Vector() * ProjectileMovementComponent->InitialSpeed;
}

void AThrowingKnife::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ASevenCharacter* SevenCharacter = Cast<ASevenCharacter>(OtherActor))
	{
		if (SevenCharacter == GetOwner())
		{
			return;
		}
		SevenCharacter->ReceivedHit(AttackInfo);
	}
	Destroy();
}

void AThrowingKnife::Tick(float DeltaTime)
{
	UE_LOG(LogTemp, Display, TEXT("[AThrowingKnife] FireInDirection"));
	Super::Tick(DeltaTime);
}


