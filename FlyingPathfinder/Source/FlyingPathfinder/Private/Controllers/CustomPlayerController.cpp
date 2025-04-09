// Fill out your copyright notice in the Description page of Project Settings.


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
