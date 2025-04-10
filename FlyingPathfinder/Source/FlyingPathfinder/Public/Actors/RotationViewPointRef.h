// Copyright(c) 2025 Gabriel Forget. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RotationViewPointRef.generated.h"

UCLASS()
class FLYINGPATHFINDER_API ARotationViewPointRef : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARotationViewPointRef();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetOwnerController(AController * Controller);
	
private:
	AController* OwnerController;
};
