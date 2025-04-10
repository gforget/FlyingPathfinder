// Copyright(c) 2025 Gabriel Forget. All Rights Reserved.


#include "Controllers/CustomPlayerController.h"

void ACustomPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ACustomPlayerController::ActivateCursorMode(bool value)
{
	bShowMouseCursor = value;
	
	if (value)
	{
		// Select mode
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
	}
	else
	{
		// Navigation mode
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
}
