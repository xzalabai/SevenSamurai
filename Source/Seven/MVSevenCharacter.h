#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "MVSevenCharacter.generated.h"

class UCapsuleComponent;
class UFloatingPawnMovement;

UCLASS()
class SEVEN_API AMVSevenCharacter : public APawn
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Sprite,Rendering,Physics,Components|Sprite", AllowPrivateAccess = "true"))
	TObjectPtr<class UPaperSpriteComponent> RenderComponent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY()
	int8 CurrentAreaID{ -1 };

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UFloatingPawnMovement* MovementComponent;

protected:
	virtual void BeginPlay() override;

public:	
	void SetCurrentAreaID(const int8 NewAreaID);
	FORCEINLINE int8 GetCurrentAreaID() const { return CurrentAreaID; };
	AMVSevenCharacter();
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
