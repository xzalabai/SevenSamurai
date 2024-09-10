#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MV_OverlapInterface.h"
#include "VV_EntityBase.generated.h"

class UCapsuleComponent;
class AMV_PlayerController;
class UGameController;

UCLASS()
class SEVEN_API AVV_EntityBase : public AActor, public IMV_OverlapInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Sprite,Rendering,Physics,Components|Sprite", AllowPrivateAccess = "true"))
	TObjectPtr<class UPaperSpriteComponent> RenderComponent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	UCapsuleComponent* CapsuleComponent;

	AMV_PlayerController* MV_PlayerController{ nullptr };
	UGameController* GameController{ nullptr };

	
public:	
	AVV_EntityBase();

protected:
	virtual void BeginPlay() override;
	virtual void OnOverlapAction() override;
};
