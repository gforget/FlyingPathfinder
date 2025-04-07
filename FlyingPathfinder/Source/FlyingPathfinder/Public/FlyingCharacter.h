// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FlyingCharacter.generated.h"
class UInputMappingContext;
class UInputAction;
class ARotationViewPointRef;

UCLASS()
class FLYINGPATHFINDER_API AFlyingCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFlyingCharacter();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookRateAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* GoUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* GoDownAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleFlyModeAction;
	
	void MoveInput(const FInputActionValue& Value);
	void LookInput(const FInputActionValue& Value);
	void LookRateInput(const FInputActionValue& Value);
	void JumpInput(const FInputActionValue& Value);
	void GoUpInput(const FInputActionValue& Value);
	void GoDownInput(const FInputActionValue& Value);
	void ToggleFlyModeInput(const FInputActionValue& Value);
	
	ARotationViewPointRef* GetRotationViewPointRef();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	float RotationRate = 100.0f;

	UPROPERTY()
	ARotationViewPointRef* RotationViewPointRef;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ARotationViewPointRef> RotationViewPointRefClass;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};


