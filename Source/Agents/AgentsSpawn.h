// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AgentsSpawn.generated.h"

class UBoxComponent;

UCLASS()
class AGENTS_API AAgentsSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAgentsSpawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Volume")
	UBoxComponent* BoxComponent;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetSpawnLocation() const;
};
