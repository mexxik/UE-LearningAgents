// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentsSpawn.h"

#include "Components/BoxComponent.h"

AAgentsSpawn::AAgentsSpawn()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->InitBoxExtent(FVector(1000.0f, 1000.0f, 20.0f));

	SetRootComponent(BoxComponent);
}

void AAgentsSpawn::BeginPlay()
{
	Super::BeginPlay();
}

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

