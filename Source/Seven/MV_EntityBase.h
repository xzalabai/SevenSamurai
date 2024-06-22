#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MV_EntityBase.generated.h"

class UCapsuleComponent;
class UMissionDA;

UCLASS()
class SEVEN_API AMV_EntityBase : public AActor
{
	GENERATED_BODY()

	friend class AMV_MissionSpawner;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeFunctionCategories = "Sprite,Rendering,Physics,Components|Sprite", AllowPrivateAccess = "true"))
	TObjectPtr<class UPaperSpriteComponent> RenderComponent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"));
	UMissionDA* MissionDA;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	AMV_EntityBase();
	const UMissionDA* GetMissionDA() const;
};
