// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "PublicEnums.h"
#include "GodView.h"
#include "SevenPlayerController.generated.h"

class AGodView;
class UGameController;

UCLASS()
class SEVEN_API ASevenPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
public:
	////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SpaceAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* WSADAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* ToggleMovementAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FireRMBAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* BlockAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SwitchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* EvadeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Special1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Special2Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Special3Action;

	void Space(const FInputActionValue& Value);
	void StopSpace(const FInputActionValue& Value);
	void Move(const FInputActionValue& Value);
	void Sprint(const FInputActionValue& Value);
	void StartMovement(const FInputActionValue& Value, const bool bSprint);
	void Look(const FInputActionValue& Value);
	void Fire(const FInputActionValue& Value);
	void ToggleMovement(const FInputActionValue& Value);
	//void FireRMBStart(const FInputActionValue& Value);
	void FireRMB(const FInputActionValue& Value, const ETriggerEvent TriggerEvent);
	void Switch(const FInputActionValue& Value);
	void Evade(const FInputActionValue& Value);
	void Special(const FInputActionValue& Value, const int8 Number);
	void BlockStart(const FInputActionValue& Value);
	void BlockEnd(const FInputActionValue& Value);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SwitchSevenCharacter(const ASevenCharacter* SevenCharacter);
	UGameController* GetGameController() const;

	ASevenCharacter* GetPossessedCharacter();

private:
	TObjectPtr<AActor> GetControlledActor();
private:
	UPROPERTY()
	AGodView* GodView;
	bool bGodView = true;
};
