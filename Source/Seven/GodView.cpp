#include "GodView.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "SevenCharacter.h"
#include "Blueprint\AIBlueprintHelperLibrary.h"


AGodView::AGodView()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(RootComponent);
}

void AGodView::Enable(bool bEnable)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0); // TODO optimize
	PC->SetShowMouseCursor(bEnable);

	if (bEnable)
	{
		ControlledCharacters.Empty();
	}

	else if (!bEnable)
	{
		ControlledCharacters.Empty();
	}
}

void AGodView::BeginPlay()
{
	Super::BeginPlay();
}

void AGodView::Move(const FInputActionValue& Value)
{
}

void AGodView::Look(const FInputActionValue& Value)
{
}

void AGodView::Space(const FInputActionValue& Value)
{
}

void AGodView::Fire(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Display, TEXT("[AGodView] Fire()"));
	// TODO: Precache this!
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	FHitResult Hit;
	FVector WorldLocation, WorldDirection;
	PC->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);

	FVector TraceStart = WorldLocation;
	FVector TraceEnd = WorldLocation + (WorldDirection * 10000.0f);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_Camera, QueryParams);
	DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 20, 5, FColor::Black, false, 3);

	// Add clicked character to the list of controlled characters
	if (TObjectPtr<ASevenCharacter> SevenCharacter = Cast<ASevenCharacter>(Hit.GetActor()))
	{
		ControlledCharacters.Add(SevenCharacter);
		return;
	}
	// Set command for them
	else if (ControlledCharacters.Num() > 0 && bHit)
	{
		for (TObjectPtr<ASevenCharacter> ControlledCharacter : ControlledCharacters)
		{
			ControlledCharacter->SetActorLocation(Hit.Location);
		}
	}

}

void AGodView::StopSpace(const FInputActionValue& Value)
{
}

