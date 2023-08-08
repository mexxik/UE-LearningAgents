// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LearningAgentsManager.h"
#include "LearningAgentsCritic.h"
#include "LearningAgentsPolicy.h"
#include "LearningAgentsTrainer.h"
#include "AgentsManager.generated.h"

class UAgentsInteractor;
class UAgentsCritic;
class UAgentsPolicy;
class UAgentsTrainer;

/**
 * 
 */
UCLASS()
class AGENTS_API AAgentsManager : public ALearningAgentsManager
{
	GENERATED_BODY()
	
private:
	bool bTrainMode = false;
	int32 StepNum = 0;
	int32 SaveEverySteps = 1000;

protected:
	FLearningAgentsCriticSettings CriticSettings;
	FLearningAgentsPolicySettings PolicySettings;
	FLearningAgentsTrainerSettings TrainerSettings;
	FLearningAgentsTrainerTrainingSettings TrainerTrainingSettings;
	FLearningAgentsTrainerGameSettings TrainerGameSettings;
	FLearningAgentsTrainerPathSettings TrainerPathSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentsManager")
	UAgentsInteractor* AgentsInteractor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentsManager")
	UAgentsCritic* AgentsCritic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentsManager")
	UAgentsPolicy* AgentsPolicy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AgentsManager")
	UAgentsTrainer* AgentsTrainer;
public:
	AAgentsManager();

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	void Setup(bool bTrain, FFilePath PoliycyWeightsPath = FFilePath());
	void LoadLastWeights();
};
