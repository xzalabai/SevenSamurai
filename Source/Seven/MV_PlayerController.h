#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PublicEnums.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "MV_PlayerController.generated.h"

class AMVSevenCharacter;

UCLASS()
class SEVEN_API AMV_PlayerController : public APlayerController
{
	GENERATED_BODY()

///// MEMBER VARIABLES
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AMVSevenCharacter* MVSevenCharacter{ nullptr };

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* WSADAction;

protected:

	UFUNCTION(BlueprintImplementableEvent)
	void DisplayCharacterShop();
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCharacterShop();
	UFUNCTION(BlueprintImplementableEvent)
	void DisplayComboShop();
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateComboShop();

///// FUNCTIONS
public:
	AMV_PlayerController();
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	void Move(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void PerformTraceToMap() const;
	void DisplayUIWindow(EUIWindow UIWindow, bool bDisplay);
	void UpdateUIWindow(EUIWindow UIWindow);
	
};
