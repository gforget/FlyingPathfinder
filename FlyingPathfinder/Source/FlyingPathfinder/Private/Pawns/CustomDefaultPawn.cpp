// Copyright(c) 2025 Gabriel Forget. All Rights Reserved.
#include "Pawns/CustomDefaultPawn.h"

#include "Controllers/CustomAIController.h"
#include "Controllers/CustomPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "ActorComponents/FlyingMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameModes/FlyingPathfinderGameMode.h"
#include "Pathfinder/FlyingPathfinderVolume.h"
#include "Pawns/FlyingCharacter.h"


// Sets default values
ACustomDefaultPawn::ACustomDefaultPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACustomDefaultPawn::BeginPlay()
{
	Super::BeginPlay();
	FlyingPathfinderGameMode = Cast<AFlyingPathfinderGameMode>(GetWorld()->GetAuthGameMode());
}

void ACustomDefaultPawn::ToggleSelectModeInput(const FInputActionValue& Value)
{
	bSelectMode = !bSelectMode;
	
	if (bSelectMode)
	{
		if (MovementComponent)
		{
			MovementComponent->SetActive(false);
			if (ACustomPlayerController* PC = Cast<ACustomPlayerController>(Controller))
			{
				PC->ActivateCursorMode(true);
			}
		}
	}
	else
	{
		if (MovementComponent)
		{
			MovementComponent->SetActive(true);
			if (ACustomPlayerController* PC = Cast<ACustomPlayerController>(Controller))
			{
				PC->ActivateCursorMode(false);
			}
		}
	}
}

void ACustomDefaultPawn::SelectOnSceneInput(const FInputActionValue& Value)
{
	if (!bSelectMode)
	{
		return;
	}
	
	// Get player controller
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!PC)
	{
		return;
	}
	
	// Get mouse position
	FVector2D MousePosition;
	if (!PC->GetMousePosition(MousePosition.X, MousePosition.Y))
	{
		return;
	}
	
	// Convert screen position to world direction
	FVector WorldLocation;
	FVector WorldDirection;
	if (PC->DeprojectScreenPositionToWorld(MousePosition.X, MousePosition.Y, WorldLocation, WorldDirection))
	{
		// Set up trace parameters
		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);
		
		FVector TraceStart = WorldLocation;
		FVector TraceEnd = WorldLocation + (WorldDirection * MaxTraceDistance);
		
		// Perform trace
		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			TraceStart,
			TraceEnd,
			ECC_Visibility,
			QueryParams
		);
		
		if (bHit)
		{
			// Hit something, store hit location
			LastTraceHitLocation = HitResult.Location;
		}
		else
		{
			// Nothing hit, use max distance
			LastTraceHitLocation = TraceEnd;
		}
		
		// if (TargetAIController)
		// {
		// 	TargetAIController->SetPositionToGo(LastTraceHitLocation);
		// }
		
		if (FlyingPathfinderGameMode)
		{
			
			AFlyingCharacter* FlyingCharacter = Cast<AFlyingCharacter>(TargetAIController->GetPawn());
			AFlyingPathfinderVolume* FlyingPathfinderVolume = FlyingPathfinderGameMode->GetVolumeAtLocation(LastTraceHitLocation);
			
			if (FlyingCharacter && FlyingPathfinderVolume)
			{
				GenericStack<UFlyingPathfindingNode*> PathToDestination = FlyingPathfinderVolume->GetPathToDestination(
					FlyingCharacter->GetActorLocation() + FlyingCharacter->FootPositionAnchor,
					LastTraceHitLocation
					);
				
				FlyingCharacter->FlyingMovementComponent->FollowFlyingPath(PathToDestination, LastTraceHitLocation);
			}
		}
	}
}

// Called every frame
void ACustomDefaultPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACustomDefaultPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}
	
	// Bind input function
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ToggleSelectModeAction != nullptr) 
			EnhancedInputComponent->BindAction(ToggleSelectModeAction, ETriggerEvent::Triggered, this, &ACustomDefaultPawn::ToggleSelectModeInput);
		
		if (SelectOnSceneAction != nullptr)
			EnhancedInputComponent->BindAction(SelectOnSceneAction, ETriggerEvent::Triggered, this, &ACustomDefaultPawn::SelectOnSceneInput);
	}
}

void ACustomDefaultPawn::RegisterAIController(ACustomAIController* NewAIController)
{
	if (!TargetAIController)
	{
		TargetAIController = NewAIController;
	}
}

