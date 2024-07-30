#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"

#include "Shield.generated.h"

class USphereComponent;
class ASevenCharacter;

UCLASS()
class SEVEN_API AShield : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* BlockCollider;

protected:
	virtual void BeginPlay() override;

public:	
	AShield();
	void AttachToSocket(USkeletalMeshComponent* PlayerMesh, FName SocketName);
};
