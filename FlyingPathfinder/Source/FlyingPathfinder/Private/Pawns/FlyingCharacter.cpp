﻿// Copyright(c) 2025 Gabriel Forget. All Rights Reserved.


#include "FlyingPathfinder/Public/Pawns/FlyingCharacter.h"

#include <string>

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FlyingPathfinder/Public/Actors/RotationViewPointRef.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ActorComponents/FlyingMovementComponent.h"
#include "Utility/DebugHeader.h"

// Sets default values
AFlyingCharacter::AFlyingCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	// Create and initialize the flying movement component
	FlyingMovementComponent = CreateDefaultSubobject<UFlyingMovementComponent>(TEXT("FlyingMovementComponent"));
}

void AFlyingCharacter::MoveInput(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (GetCharacterMovement()->GetMovementName() == "Walking" || GetCharacterMovement()->GetMovementName() == "Falling" )
	{
		AddMovementInput(GetActorRightVector(), MovementVector.X);
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
	}
	else if (GetCharacterMovement()->GetMovementName() == "Flying")
	{
		AddMovementInput(GetRotationViewPointRef()->GetActorRightVector(), MovementVector.X);
		AddMovementInput(GetRotationViewPointRef()->GetActorForwardVector(), MovementVector.Y);
	}
}

ARotationViewPointRef* AFlyingCharacter::GetRotationViewPointRef()
{
	return RotationViewPointRef;
}

void AFlyingCharacter::LookInput(const FInputActionValue& Value)
{
	FVector2D RotationVector = Value.Get<FVector2D>();
	AddControllerYawInput(RotationVector.X);
	AddControllerPitchInput(RotationVector.Y);
}

void AFlyingCharacter::LookRateInput(const FInputActionValue& Value)
{
	FVector2D RotationVector = Value.Get<FVector2D>();
	AddControllerYawInput(RotationVector.X * RotationRate * GetWorld()->GetDeltaSeconds());
	AddControllerPitchInput(RotationVector.Y * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AFlyingCharacter::JumpInput(const FInputActionValue& Value)
{
	const FString MovementMode = GetCharacterMovement()->GetMovementName();
	if (MovementMode == "Walking")
	{
		ACharacter::Jump();
	}	
}

void AFlyingCharacter::GoUpInput(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->GetMovementName() == "Flying")
	{
		float MovementValue = Value.Get<float>();
		AddMovementInput(GetActorUpVector(), MovementValue);
		DebugScreen("Go up" + FString::Printf(TEXT("%f"), MovementValue), FColor::Cyan);
	}
}

void AFlyingCharacter::GoDownInput(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->GetMovementName() == "Flying")
	{
		float MovementValue = Value.Get<float>();
		AddMovementInput(-GetActorUpVector(), MovementValue);
	}
}

void AFlyingCharacter::ToggleFlyModeInput(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->GetMovementName() == "Walking" || GetCharacterMovement()->GetMovementName() == "Falling")
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}
	else if (GetCharacterMovement()->GetMovementName() == "Flying")
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

// Called when the game starts or when spawned
void AFlyingCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//RotationViewPointRef 
	RotationViewPointRef = GetWorld()->SpawnActor<ARotationViewPointRef>(
		RotationViewPointRefClass,
		FVector::ZeroVector,
		FRotator::ZeroRotator
	);

	RotationViewPointRef->SetOwnerController(GetController());
	RotationViewPointRef->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform, NAME_None);
}

void AFlyingCharacter::PostActorCreated()
{
	Super::PostActorCreated();
	GenerateEditorAnchorPositionVisualisation();
}

void AFlyingCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	GenerateEditorAnchorPositionVisualisation();
}

void AFlyingCharacter::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	GenerateEditorAnchorPositionVisualisation();
}

void AFlyingCharacter::GenerateEditorAnchorPositionVisualisation() const
{
	if (const UWorld* World = GetWorld())
	{
		if (World->WorldType == EWorldType::EditorPreview)
		{
			UKismetSystemLibrary::FlushPersistentDebugLines(this);

			const FVector ActorLocation = GetActorLocation();
			DrawDebugSphere(GetWorld(), ActorLocation + FootPositionAnchor, 5.0f, 12, FColor::Purple, true, 0.0f, 0, 0.0f);
		}
	}
}

// Called every frame
void AFlyingCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AFlyingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
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
		if (MoveAction != nullptr) EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFlyingCharacter::MoveInput);
		if (LookAction != nullptr) EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFlyingCharacter::LookInput);
		if (LookRateAction != nullptr) EnhancedInputComponent->BindAction(LookRateAction, ETriggerEvent::Triggered, this, &AFlyingCharacter::LookRateInput);
		if (JumpAction != nullptr) EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AFlyingCharacter::JumpInput);
		if (GoUpAction != nullptr) EnhancedInputComponent->BindAction(GoUpAction, ETriggerEvent::Triggered, this, &AFlyingCharacter::GoUpInput);
		if (GoDownAction != nullptr) EnhancedInputComponent->BindAction(GoDownAction, ETriggerEvent::Triggered, this, &AFlyingCharacter::GoDownInput);
		if (ToggleFlyModeAction != nullptr) EnhancedInputComponent->BindAction(ToggleFlyModeAction, ETriggerEvent::Triggered, this, &AFlyingCharacter::ToggleFlyModeInput);
	}
}

