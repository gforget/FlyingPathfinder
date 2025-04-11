// // Copyright(c) 2025 Gabriel Forget. All Rights Reserved.

#include "ActorComponents/FlyingMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Pathfinder/FlyingPathfindingNode.h"
#include "Pawns/FlyingCharacter.h"
#include "Utility/DebugHeader.h"

// Sets default values for this component's properties
UFlyingMovementComponent::UFlyingMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFlyingMovementComponent::FollowFlyingPath(const GenericStack<UFlyingPathfindingNode*>& NewPath, const FVector& NewFinalDestination)
{
	FlyingPath.Clear();
	FlyingPath = NewPath;

	FinalDestinationPoint = NewFinalDestination;
	FlyingCharacterOwner->GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	CurrentDestination = FlyingPath.Pop();
}

// Called when the game starts
void UFlyingMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	FlyingCharacterOwner = Cast<AFlyingCharacter>(GetOwner());
	
	if (FlyingCharacterOwner == nullptr)
	{
		DebugScreen("Owner not a FlyingCharacter", FColor::Red);
	}
}

// Called every frame
void UFlyingMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CurrentDestination != nullptr)
	{
		FVector DirectionToDestination = (CurrentDestination->GetComponentLocation() - FlyingCharacterOwner->FootPositionAnchor) - (FlyingCharacterOwner->GetActorLocation());
		float DistanceToDestination = DirectionToDestination.Size();
		DirectionToDestination.Normalize();
		
		// Set velocity directly on the character movement component
		FlyingCharacterOwner->GetCharacterMovement()->Velocity = DirectionToDestination * FlyingCharacterOwner->GetCharacterMovement()->MaxFlySpeed;

		// Calculate the destination rotation (yaw only)
		FRotator CurrentRotation = FlyingCharacterOwner->GetActorRotation();
		FVector FinalDestinationDirection = FinalDestinationPoint - FlyingCharacterOwner->GetActorLocation();
		FinalDestinationDirection.Normalize();
		
		FRotator FinalDestinationRotation = FinalDestinationDirection.Rotation();
		
		// We only want to rotate on the yaw axis, so keep the current pitch and roll
		FinalDestinationRotation.Pitch = CurrentRotation.Pitch;
		FinalDestinationRotation.Roll = CurrentRotation.Roll;
		
		// Interpolate the rotation for smooth turning
		FRotator NewRotation = FMath::RInterpTo(
			CurrentRotation,
			FinalDestinationRotation,
			DeltaTime,
			RotationSpeed
		);
		
		// Apply the rotation to the character
		FlyingCharacterOwner->SetActorRotation(NewRotation);

		if (DistanceToDestination < 10.0f)
		{
			if (FlyingPath.Num() > 0)
			{
				CurrentDestination = FlyingPath.Pop();
			}
			else
			{
				CurrentDestination = nullptr;
				FlyingCharacterOwner->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
			}
		}
	}
}

