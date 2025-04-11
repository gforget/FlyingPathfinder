// Copyright(c) 2025 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/DefaultPawn.h"
#include "CustomDefaultPawn.generated.h"

class AFlyingPathfinderGameMode;
class ACustomAIController;
class UInputMappingContext;
class UInputAction;

UCLASS()
class FLYINGPATHFINDER_API ACustomDefaultPawn : public ADefaultPawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACustomDefaultPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleSelectModeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SelectOnSceneAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trace")
	float MaxTraceDistance = 10000.0f;

	void ToggleSelectModeInput(const FInputActionValue& Value);
	void SelectOnSceneInput(const FInputActionValue& Value);

	UPROPERTY()
	AFlyingPathfinderGameMode* FlyingPathfinderGameMode;
	
	UPROPERTY()
	ACustomAIController* TargetAIController;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void RegisterAIController(ACustomAIController* NewAIController);
	
private:
	
	UPROPERTY()
	bool bSelectMode = false;
	
	UPROPERTY()
	FVector LastTraceHitLocation;
};
