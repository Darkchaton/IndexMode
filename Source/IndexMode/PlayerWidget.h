// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class INDEXMODE_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
	
	public:
    
    UFUNCTION(BlueprintImplementableEvent)
    void SetPromptF(bool visible);
};
