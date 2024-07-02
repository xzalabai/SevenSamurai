#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MV_OverlapInterface.h"
#include "MV_EntityBase.generated.h"

class UCapsuleComponent;
class UMissionDA;
class AMV_Map;

UCLASS()
class SEVEN_API AMV_EntityBase : public APawn, public IMV_OverlapInterface
{
	GENERATED_BODY()

	friend class AMV_Map;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Sprite,Rendering,Physics,Components|Sprite", AllowPrivateAccess = "true"))
	TObjectPtr<class UPaperSpriteComponent> RenderComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Sprite,Rendering,Physics,Components|Sprite", AllowPrivateAccess = "true"))
	TObjectPtr<class UPaperSprite> AfterMissionImage;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	UMissionDA* MissionDA;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	AMV_EntityBase();
	UMissionDA* GetMissionDA() const;
	virtual void OnOverlapAction() override;

};
