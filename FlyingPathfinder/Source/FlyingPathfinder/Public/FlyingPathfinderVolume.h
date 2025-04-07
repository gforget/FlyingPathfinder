// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Volume.h"
#include "FlyingPathfinderVolume.generated.h"

UCLASS()
class FLYINGPATHFINDER_API AFlyingPathfinderVolume : public AVolume
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlyingPathfinderVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when actor is moved, rotated or scaled in the editor
	virtual void OnConstruction(const FTransform& Transform) override;
	
#if WITH_EDITOR
	// Called when a property is changed in the editor
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	// Called when actor is moved in the editor
	virtual void PostEditMove(bool bFinished) override;
	
	// Updates the debug visualization
	void UpdateDebugVisualization();
#endif
	
	// Debug sphere properties
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bShowDebugSphere = true;
	
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (EditCondition = "bShowDebugSphere"))
	float DebugSphereRadius = 20.0f;
	
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (EditCondition = "bShowDebugSphere"))
	FColor DebugSphereColor = FColor::Green;
	
	// Grid properties
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (EditCondition = "bShowDebugSphere", ClampMin = "50.0"))
	float GridSpacing = 200.0f;
	
	UPROPERTY(EditAnywhere, Category = "Debug", meta = (EditCondition = "bShowDebugSphere"))
	bool bLimitGridToBounds = true;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
