// Copyright Epic Games, Inc. All Rights Reserved.

#include "AgentsGameMode.h"
#include "AgentsCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "AgentsSpawn.h"
#include "Agents/RL/AgentsManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "EngineUtils.h"
#include "Agents/HUD/AgentsMenu.h"
#include "Kismet/GameplayStatics.h"

AAgentsGameMode::AAgentsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		// DefaultPawnClass = PlayerPawnBPClass.Class;
		AgentClass = PlayerPawnBPClass.Class;
	}

	static ConstructorHelpers::FClassFinder<AAgentsManager> AgentsManagerClassFinder(TEXT("/Game/ThirdPerson/Blueprints/RL/BP_AgentsManager"));
	if (AgentsManagerClassFinder.Succeeded())
	{
		AgentsManagerClass = AgentsManagerClassFinder.Class;
	}

	static ConstructorHelpers::FClassFinder<UAgentsMenu> FoundMenuClass(TEXT("/Game/ThirdPerson/Blueprints/HUD/WBP_Menu"));
	if (FoundMenuClass.Class != NULL)
	{
		MenuClass = FoundMenuClass.Class;
	}

	

	// AgentsManager = CreateDefaultSubobject<AAgentsManager>(TEXT("AgentsManager"));
}

void AAgentsGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (MenuClass)
	{
		Menu = CreateWidget<UAgentsMenu>(GetWorld(), MenuClass);

		Menu->SetupMenu();
	}
}

void AAgentsGameMode::StartTraining()
{
	if (AgentsManagerClass && !AgentsManager)
	{
		AgentsManager = GetWorld()->SpawnActor<AAgentsManager>(AgentsManagerClass);
	}

	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		for (TActorIterator<AAgentsSpawn> It(World); It; ++It)
		{
			AAgentsSpawn* AgentsSpawn = *It;

			for (int32 i = 0; i < AgentsManager->GetMaxAgentNum(); i++)
			{

				// Get the spawn location from the AgentsSpawn actor
				FVector SpawnLocation = AgentsSpawn->GetSpawnLocation();
				SpawnLocation.Z = 91.0f;

				// Define the rotation for the new actor (adjust according to your needs)
				FRotator SpawnRotation = FRotator::ZeroRotator;

				// Define the spawn parameters (adjust according to your needs)
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				// Spawn the character
				AAgentsCharacter* NewCharacter = World->SpawnActor<AAgentsCharacter>(AgentClass, SpawnLocation, SpawnRotation, SpawnParameters);
				if (NewCharacter)
				{
					NewCharacter->Respawn();
				}

			}

			break;
		}
	}

	if (AgentsManager)
	{
		AgentsManager->Setup(true);
	}
}

void AAgentsGameMode::ContinueTraining()
{
	if (AgentsManagerClass && !AgentsManager)
	{
		AgentsManager = GetWorld()->SpawnActor<AAgentsManager>(AgentsManagerClass);
	}

	if (AgentsManager)
	{
		AgentsManager->LoadLastWeights();
	}

	StartTraining();
}

void AAgentsGameMode::Play(FString Weights)
{
	if (AgentsManagerClass)
	{
		AgentsManager = GetWorld()->SpawnActor<AAgentsManager>(AgentsManagerClass);
	}

	UWorld* World = GetWorld();
	if (World != nullptr)
	{
		FString Tag = FString::Printf(TEXT("InferenceSpawn"));
		TArray<AActor*> FoundActors;

		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(*Tag), FoundActors);

		if (FoundActors.Num() > 0)
		{
			AAgentsSpawn* AgentsSpawn = Cast<AAgentsSpawn>(FoundActors[0]);

			FVector SpawnLocation = AgentsSpawn->GetSpawnLocation();
			SpawnLocation.Z = 91.0f;

			// Define the rotation for the new actor (adjust according to your needs)
			FRotator SpawnRotation = FRotator::ZeroRotator;

			// Define the spawn parameters (adjust according to your needs)
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			// Spawn the character
			AAgentsCharacter* NewCharacter = World->SpawnActor<AAgentsCharacter>(AgentClass, SpawnLocation, SpawnRotation, SpawnParameters);
			if (NewCharacter)
			{
				NewCharacter->SetInferenceMode(true);
				NewCharacter->Respawn();
			}
		}
	}

	if (AgentsManager)
	{
		FFilePath FilePath;
		FilePath.FilePath = FPaths::ProjectDir() + Weights;

		AgentsManager->Setup(false, FilePath);
	}
}

void AAgentsGameMode::StartPlaying()
{
	Play("Weights/Runner.policy");
}

void AAgentsGameMode::StartPlayingPretrained()
{
	Play("Weights/RunnerPretrained.policy");
}
