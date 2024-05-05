#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AICharacter.generated.h"

class ASevenCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SEVEN_API UAICharacter : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAICharacter();
	void MoveTo(bool bToSevenCharacter, bool bBlockingStance);
	ASevenCharacter* SelectEnemy();
	const FVector GetRandomGuardPoint();

protected:
	virtual void BeginPlay() override;
};
