#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AITypes.h"
#include "Navigation/PathFollowingComponent.h"
#include "AICharacter.generated.h"

class ASevenCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEVEN_API UAICharacter : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAICharacter();
	
	void MoveTo(const FVector& Position);
	void RequestFinished(FAIRequestID x, const FPathFollowingResult& xx);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsMovementFinished();
	UFUNCTION(BlueprintCallable)
	void ResetMovementFinished();
	void FollowSevenCharacter(const ASevenCharacter* SevenCharacter);
	
private:
	bool bMovementFinished{ false };

protected:
	virtual void BeginPlay() override;
};
