#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ControllableInterface.h"
#include "GodView.generated.h"

class ASevenCharacter;

UCLASS()
class SEVEN_API AGodView : public AActor, public IControllableInterface
{
	GENERATED_BODY()
	
public:	
	AGodView();
	void Enable(bool bEnable);

protected:
	virtual void BeginPlay() override;
	// Controllable Int
	virtual void Move(const FInputActionValue& Value) override;
	virtual void Look(const FInputActionValue& Value) override;
	virtual void Space(const FInputActionValue& Value) override;
	virtual void Fire(const FInputActionValue& Value) override;
	virtual void Block(bool bEnable) override;
	virtual void StopSpace(const FInputActionValue& Value) override;
	virtual void AttackStart() override;
	virtual void Evade(const FInputActionValue& Value) override;
	virtual void Special(int ID) override;


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	TArray<TObjectPtr<ASevenCharacter>> ControlledCharacters;
};
