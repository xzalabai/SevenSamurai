#include "MV_MissionSpawner.h"
#include "PaperSpriteComponent.h"
#include "MV_EntityBase.h"
#include "MV_Map.h"
#include "Kismet\KismetMathLibrary.h"
#include "Kismet\GameplayStatics.h"

AMV_MissionSpawner::AMV_MissionSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AMV_MissionSpawner::BeginPlay()
{
	Super::BeginPlay();
	Map = Cast<AMV_Map>(UGameplayStatics::GetActorOfClass(this, AMV_Map::StaticClass()));
	FVector Pos = GetRandomPointOnMap(Map->GetRenderComponent());
	UE_LOG(LogTemp, Error, TEXT("MyCharacter's Location is %s"),*Pos.ToString());

	UWorld* World = GetWorld();
	if (World)
	{
		// Specify the location of the debug point
		FVector Location = Pos;

		// Define the size of the point
		float PointSize = 10.0f;

		// Define the color of the point
		FColor PointColor = FColor::Red;

		// Define the duration the point will be displayed
		float Duration = 5.0f;

		// Draw the debug point
		DrawDebugPoint(
			World,
			Location,
			PointSize,
			PointColor,
			false,  // Persistent lines
			Duration
		);
	}

	UE_LOG(LogTemp, Error, TEXT("MyCharacter's Location is %d"), AvailableMissions.Num());

	FTransform T{ FRotator(0, -180, -90), FVector(Pos.X, Pos.Y, Pos.Z + 1), FVector(1,1,1)};
	AMV_EntityBase* NewMission = GetWorld()->SpawnActorDeferred<AMV_EntityBase>(MissionClass, T);
	NewMission->MissionDA = AvailableMissions[0];
	NewMission->FinishSpawning(T);

	ActiveMissions.Add(NewMission);
}

void AMV_MissionSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AMV_MissionSpawner::GetRandomPointOnMap(UPaperSpriteComponent* SpriteComponent) const
{
	if (!SpriteComponent)
	{
		return FVector::ZeroVector;
	}

	// Get the bounds of the sprite
	FBoxSphereBounds SpriteBounds = SpriteComponent->CalcBounds(SpriteComponent->GetComponentTransform());

	// Get the center and extent of the bounding box
	FVector Center = SpriteBounds.Origin;
	FVector Extent = SpriteBounds.BoxExtent;

	// Generate a random point within the bounding box
	FVector RandomPoint = UKismetMathLibrary::RandomPointInBoundingBox(Center, Extent);

	// Since this is a 2D sprite, we should ensure the Z value matches the sprite's Z location
	RandomPoint.Z = Center.Z;

	return RandomPoint;
}