// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AgentsTarget.generated.h"

class UBoxComponent;

UCLASS()
class AGENTS_API AAgentsTarget : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAgentsTarget();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Volume")
	UBoxComponent* BoxComponent;

	// Effects
	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystemComponent* Effect1;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystemComponent* Effect2;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystemComponent* Effect3;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	UParticleSystemComponent* Effect4;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
