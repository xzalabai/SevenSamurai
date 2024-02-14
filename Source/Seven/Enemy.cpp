// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet\GameplayStatics.h"

AEnemy::AEnemy()
{
	bEnemy = true;
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASevenCharacter::StaticClass(), FoundActors);

	for (AActor* &Act : FoundActors)
	{
		if (ASevenCharacter* PossibleCharacter = Cast<ASevenCharacter>(Act))
		{
			if (PossibleCharacter->CanBePossessed())
			{
				MainCharacter = PossibleCharacter;
				UE_LOG(LogTemp, Warning, TEXT("AEnemy.Found Character %s"), *MainCharacter->GetName());
				break;
			}
		}
	}

	// Only temporary
	SetActorTickInterval(1.5f);

}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MainCharacter)
	{
		RotateTowards(MainCharacter);
	}
}
