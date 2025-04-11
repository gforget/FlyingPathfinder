// // Copyright(c) 2025 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Utility/GenericStack.h"
#include "FlyingMovementComponent.generated.h"


class AFlyingCharacter;
class UFlyingPathfindingNode;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FLYINGPATHFINDER_API UFlyingMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFlyingMovementComponent();
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flying Movement Test")
	// float TestProperty = 100.0f;

	void FollowFlyingPath(const GenericStack<UFlyingPathfindingNode*>& NewPath, const FVector& NewFinalDestination);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	GenericStack<UFlyingPathfindingNode*> FlyingPath;

	UPROPERTY()
	FVector FinalDestinationPoint;
	
	UPROPERTY()
	AFlyingCharacter* FlyingCharacterOwner;

	UPROPERTY()
	UFlyingPathfindingNode* CurrentDestination;
	
	// Speed at which the character rotates towards the destination (degrees per second)
	UPROPERTY(EditAnywhere, Category = "Flying Movement", meta = (AllowPrivateAccess = "true"))
	float RotationSpeed = 5.0f;
};
