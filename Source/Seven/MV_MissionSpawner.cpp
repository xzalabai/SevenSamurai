#include "MV_MissionSpawner.h"
#include "PaperSpriteComponent.h"
#include "MV_EntityBase.h"
#include "MV_Map.h"
#include "Kismet\KismetMathLibrary.h"
#include "Kismet\GameplayStatics.h"
#include "GameController.h"

AMV_MissionSpawner::AMV_MissionSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AMV_MissionSpawner::BeginPlay()
{
	Super::BeginPlay();
	Map = Cast<AMV_Map>(UGameplayStatics::GetActorOfClass(this, AMV_Map::StaticClass()));
	FVector Pos = GetRandomPointOnMap(Map->GetRenderComponent());

	const UGameInstance* GameInstance = Cast<UGameInstance>(GetWorld()->GetGameInstance());
	UGameController* GameController = Cast<UGameController>(GameInstance->GetSubsystem<UGameController>());
	GameController->OnMissionUpdate.AddUObject(this, &AMV_MissionSpawner::OnMissionUpdate);


	UWorld* World = GetWorld();
	FVector Location = Pos;
	float PointSize = 10.0f;
	FColor PointColor = FColor::Red;
	float Duration = 5.0f;
	DrawDebugPoint(World,Location,PointSize,PointColor,false, Duration);

	FTransform T{ FRotator(0, -180, -90), FVector(Pos.X, Pos.Y, Pos.Z + 1), FVector(1,1,1)};
	AMV_EntityBase* NewMission = GetWorld()->SpawnActorDeferred<AMV_EntityBase>(MissionClass, T);
	NewMission->MissionDA = AvailableMissions[0];
	NewMission->FinishSpawning(T);

	DisplayedMissions.Add(NewMission);
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

void AMV_MissionSpawner::OnMissionUpdate(const UMissionDA* Mission, const EMissionStatus Status)
{

}
