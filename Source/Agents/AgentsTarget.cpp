// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentsTarget.h"

#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"

AAgentsTarget::AAgentsTarget()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->InitBoxExtent(FVector(100.0f, 100.0f, 20.0f));
	BoxComponent->SetGenerateOverlapEvents(true);
	BoxComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	SetRootComponent(BoxComponent);

	Effect1 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect1"));
	Effect1->SetupAttachment(BoxComponent);

	Effect2 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect2"));
	Effect2->SetupAttachment(BoxComponent);

	Effect3 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect3"));
	Effect3->SetupAttachment(BoxComponent);

	Effect4 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Effect4"));
	Effect4->SetupAttachment(BoxComponent);
}

void AAgentsTarget::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAgentsTarget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

