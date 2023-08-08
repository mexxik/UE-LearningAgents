// Fill out your copyright notice in the Description page of Project Settings.


#include "AgentsManager.h"

#include "AgentsInteractor.h"
#include "AgentsCritic.h"
#include "AgentsPolicy.h"
#include "AgentsTrainer.h"
#include "Kismet/GameplayStatics.h"
#include "Agents/AgentsCharacter.h"

AAgentsManager::AAgentsManager()
{
	PrimaryActorTick.bCanEverTick = true;

	AgentsInteractor = CreateDefaultSubobject<UAgentsInteractor>(TEXT("AgentsInteractor"));
	AgentsCritic = CreateDefaultSubobject<UAgentsCritic>(TEXT("AgentsCritic"));
	AgentsPolicy = CreateDefaultSubobject<UAgentsPolicy>(TEXT("AgentsPolicy"));
	AgentsTrainer = CreateDefaultSubobject<UAgentsTrainer>(TEXT("AgentsTrainer"));
}

void AAgentsManager::BeginPlay()
{
	Super::BeginPlay();
}

void AAgentsManager::PostInitializeComponents()
{	
	Super::PostInitializeComponents();
}

void AAgentsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bTrainMode)
	{
		if (AgentsTrainer && AgentsTrainer->IsSetup())
		{
			AgentsTrainer->RunTraining(
				TrainerTrainingSettings,
				TrainerGameSettings,
				TrainerPathSettings,
				CriticSettings
			);

			if (StepNum != 0 && StepNum % SaveEverySteps == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Saving model..."));

				FFilePath PolicyFilePath;
				PolicyFilePath.FilePath = FPaths::ProjectDir() + "Weights/Runner.policy";
				AgentsPolicy->SavePolicyToSnapshot(PolicyFilePath);

				FFilePath CriticFilePath;
				CriticFilePath.FilePath = FPaths::ProjectDir() + "Weights/Runner.critic";
				AgentsCritic->SaveCriticToSnapshot(CriticFilePath);
			}

			StepNum++;
		}
	}
	else
	{
		if (AgentsPolicy)
		{
			AgentsPolicy->RunInference();
		}
	}
}

void AAgentsManager::Setup(bool bTrain, FFilePath PoliycyWeightsPath)
{
	bTrainMode = bTrain;

	if (AgentsInteractor)
	{
		AgentsInteractor->SetManager(this);
		AgentsInteractor->SetupInteractor();

		PolicySettings.LayerNum = 6;
		//PolicySettings.HiddenLayerSize = 512;
		AgentsPolicy->SetManager(this);
		AgentsPolicy->SetupPolicy(AgentsInteractor, PolicySettings);

		if (bTrainMode)
		{
			CriticSettings.LayerNum = 6;
			//CriticSettings.HiddenLayerSize = 512;
			AgentsCritic->SetManager(this);
			AgentsCritic->SetupCritic(AgentsInteractor, CriticSettings);

			TrainerSettings.MaxStepNum = 200;
			AgentsTrainer->SetManager(this);
			AgentsTrainer->SetupTrainer(AgentsInteractor, AgentsPolicy, AgentsCritic, TrainerSettings);

			SaveEverySteps = TrainerSettings.MaxStepNum * 10;
		}
		else
		{
			AgentsPolicy->LoadPolicyFromSnapshot(PoliycyWeightsPath);
		}


		UWorld* World = GetWorld();
		if (World)
		{
			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsOfClass(World, AAgentsCharacter::StaticClass(), FoundActors);
			for (auto Actor : FoundActors)
			{
				int32 AgentId = AddAgent(Actor);
			}
		}
	}

}

void AAgentsManager::LoadLastWeights()
{
	FFilePath PolicyFilePath;
	PolicyFilePath.FilePath = FPaths::ProjectDir() + "Weights/Runner.policy";
	AgentsPolicy->LoadPolicyFromSnapshot(PolicyFilePath);

	FFilePath CriticFilePath;
	CriticFilePath.FilePath = FPaths::ProjectDir() + "Weights/Runner.critic";
	AgentsCritic->LoadCriticFromSnapshot(CriticFilePath);
}

