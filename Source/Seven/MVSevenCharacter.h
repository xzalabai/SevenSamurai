#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "MVSevenCharacter.generated.h"

class UCapsuleComponent;
class UFloatingPawnMovement;
class UPaperSprite;

UCLASS()
class SEVEN_API AMVSevenCharacter : public APawn
{
	GENERATED_BODY()

	friend class AMV_Map;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Sprite,Rendering,Physics,Components|Sprite", AllowPrivateAccess = "true"))
	TObjectPtr<class UPaperSpriteComponent> RenderComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Sprite,Rendering,Physics,Components|Sprite", AllowPrivateAccess = "true"))
	TObjectPtr<UPaperSprite> CampImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Sprite,Rendering,Physics,Components|Sprite", AllowPrivateAccess = "true"))
	TObjectPtr<UPaperSprite> CharacterImage;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY()
	int8 CurrentAreaID{ -1 };

	UPROPERTY()
	uint16 Stamina{ 0 };

	UPROPERTY()
	uint16 AccumulatedHPGained{ 0 };

	bool bIsCamp { false };

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UFloatingPawnMovement* MovementComponent;

private:
	void UpdateImage(UPaperSprite* NewSprite) const;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:	
	void SetCurrentAreaID(const int8 NewAreaID);
	FORCEINLINE int8 GetCurrentAreaID() const { return CurrentAreaID; };
	FORCEINLINE bool IsCamp() const { return bIsCamp; };
	AMVSevenCharacter();
	UFUNCTION(BlueprintImplementableEvent)
	void OnCamp(bool bEnable);
	void UpdateSevenCharactersHP();
	UFUNCTION(BlueprintCallable)
	void Camp(bool bEnable);
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
