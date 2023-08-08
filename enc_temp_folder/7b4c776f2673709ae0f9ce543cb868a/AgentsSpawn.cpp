// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentsSpawn.h"

#include "Components/BoxComponent.h"

// Sets default values
AAgentsSpawn::AAgentsSpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->InitBoxExtent(FVector(1000.0f, 1000.0f, 20.0f));

	SetRootComponent(BoxComponent);
}

// Called when the game starts or when spawned
void AAgentsSpawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAgentsSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector AAgentsSpawn::GetSpawnLocation() const
{
	FBox BoundingBox = GetComponentsBoundingBox();
	FVector RandomPoint = FMath::RandPointInBox(BoundingBox);

	return RandomPoint;
}

