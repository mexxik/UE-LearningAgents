// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "AgentsAgentWidgetComponent.generated.h"

/**
 * 
 */
UCLASS()
class AGENTS_API UAgentsAgentWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	void SetText(FString Text);
	
};
