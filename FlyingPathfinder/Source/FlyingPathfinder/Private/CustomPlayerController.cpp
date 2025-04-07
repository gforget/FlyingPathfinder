// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerController.h"

void ACustomPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ACustomPlayerController::ActivateCursorMode(bool value)
{
	bShowMouseCursor = value;
	
	if (value)
	{
		// Use GameAndUI for cursor mode
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
	}
	else
	{
		// Use GameOnly for normal movement/camera control
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
}
