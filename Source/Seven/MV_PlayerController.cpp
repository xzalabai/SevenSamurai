#include "MV_PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "MVSevenCharacter.h"
#include "MV_AIController.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "MV_Map.h"
#include "VV_Map.h"
#include "Kismet\GameplayStatics.h"

AMV_PlayerController::AMV_PlayerController()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Scene Component"));
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));

	RootComponent = SceneComponent;
	CameraBoom->SetupAttachment(RootComponent);
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
}

void AMV_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	MVSevenCharacter = Cast<AMVSevenCharacter>(UGameplayStatics::GetActorOfClass(this, AMVSevenCharacter::StaticClass()));
}

void AMV_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		//Moving
		EnhancedInputComponent->BindAction(WSADAction, ETriggerEvent::Triggered, this, &AMV_PlayerController::Move);
	}
}

void AMV_PlayerController::Move(const FInputActionValue& Value)
{
}

void AMV_PlayerController::PerformTraceToMap() const
{
	if (MVSevenCharacter->IsCamp())
	{
		return;
	}

	FHitResult HitResult;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	bool bHit = GetHitResultUnderCursorForObjects(ObjectTypes, false, HitResult);

	if (bHit)
	{
		MVSevenCharacter->bIsMoving = true;
		AMV_AIController* MV_AISevenCharacterController = Cast<AMV_AIController>(MVSevenCharacter->GetController());
		MV_AISevenCharacterController->MoveCharacterTo(HitResult.ImpactPoint);
	}
}
