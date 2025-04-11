// Copyright(c) 2025 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FlyingCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class ARotationViewPointRef;
class UFlyingMovementComponent;

UCLASS()
class FLYINGPATHFINDER_API AFlyingCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFlyingCharacter();
	
	ARotationViewPointRef* GetRotationViewPointRef();

	UPROPERTY(EditDefaultsOnly, Category="Position Reference")
	FVector FootPositionAnchor = FVector(0.0f, 0.0f, 25.0f);
	
	// Custom flying movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UFlyingMovementComponent* FlyingMovementComponent;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostActorCreated() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditMove(bool bFinished) override;
	void GenerateEditorAnchorPositionVisualisation() const;
#endif
	
	UPROPERTY(EditAnywhere)
	float RotationRate = 100.0f;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ARotationViewPointRef> RotationViewPointRefClass;
	
	UPROPERTY()
	ARotationViewPointRef* RotationViewPointRef;
	
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
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};


