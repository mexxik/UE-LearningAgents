// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AgentsGameMode.generated.h"

class AAgentsCharacter;
class AAgentsManager;
class UAgentsMenu;

UCLASS(minimalapi)
class AAgentsGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	UPROPERTY()
	AAgentsManager* AgentsManager;

	UPROPERTY()
	UAgentsMenu* Menu;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Agents")
	TSubclassOf<AAgentsCharacter> AgentClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Agents")
	TSubclassOf<AAgentsManager> AgentsManagerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UAgentsMenu> MenuClass;

public:
	AAgentsGameMode();

	virtual void BeginPlay() override;

	void StartTraining();
	void ContinueTraining();
	void StartPlaying();
};



