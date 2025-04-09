// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyingPathfinder/Public/Actors/RotationViewPointRef.h"


// Sets default values
ARotationViewPointRef::ARotationViewPointRef()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ARotationViewPointRef::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARotationViewPointRef::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (OwnerController)
	{
		FVector Location;
		FRotator Rotation;
	
		OwnerController->GetPlayerViewPoint(Location, Rotation);
		this->SetActorRotation(Rotation);
	}
}

void ARotationViewPointRef::SetOwnerController(AController* Controller)
{
	OwnerController = Controller;
}

