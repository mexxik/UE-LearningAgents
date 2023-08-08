// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AgentsMenu.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class AGENTS_API UAgentsMenu : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY(meta = (BindWidget))
	UButton* PlayButton;

	UPROPERTY(meta = (BindWidget))
	UButton* TrainButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ContinueTrainButton;

	UFUNCTION()
	void PlayButtonClicked();

	UFUNCTION()
	void TrainButtonClicked();

	UFUNCTION()
	void ContinueTrainingButtonClicked();

	void MenuTearDown();

protected:
	virtual bool Initialize() override;

public:
	void SetupMenu();
};
