// Fill out your copyright notice in the Description page of Project Settings.


#include "ArcherEnemy.h"
#include "AttackComponent.h"
#include "Kismet\KismetMathLibrary.h"

void AArcherEnemy::BeginPlay()
{
	Super::BeginPlay();
}

const FVector AArcherEnemy::GetRandomPointAroundCharacter(const ASevenCharacter* const SevenCharacter)
{
	const FVector EnemyPosition = SevenCharacter->GetActorLocation();
	const FVector EnemyForwardVector = SevenCharacter->GetActorForwardVector();

	const int Left = FMath::RandBool() ? (-1) : 1;
	//const int RandomOffsetX = FMath::RandRange(500, 1000);
	const int RandomOffsetX = FMath::RandRange(400, 800);
	const int RandomOffsetY = FMath::RandRange(400, 800) * Left;

	const FVector EnemyForwardVectorOffset = FVector(EnemyForwardVector.X + RandomOffsetX, EnemyForwardVector.Y + RandomOffsetY, EnemyForwardVector.Z);

	const FVector FinalDestination = FVector(EnemyPosition.X + EnemyForwardVectorOffset.X, EnemyPosition.Y + EnemyForwardVectorOffset.Y, EnemyPosition.Z);
	FVector SphereLocation = FinalDestination; // Example location
	float SphereRadius = 50.0f; // Radius of the sphere
	FColor SphereColor = FColor(uniqueID*uniqueID, uniqueID * uniqueID, uniqueID * uniqueID); // Example color
	float SphereLifeTime = 5.0f; // Lifetime of 5 seconds
	DrawDebugSphere(
		GetWorld(),                    // The world context
		SphereLocation,           // Center of the sphere
		SphereRadius,             // Radius of the sphere
		12,                       // Number of segments (more segments = smoother sphere)
		SphereColor,              // Color of the sphere
		true,                    // Whether the sphere is persistent (doesn't disappear)
		SphereLifeTime,           // Lifetime before the sphere disappears (if not persistent)
		0,                        // Depth priority (0 = default)
		1.0f                      // Line thickness
	);
	return FinalDestination;
}
