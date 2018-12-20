// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AsteroidWidget.generated.h"

/**
 * 
 */
UCLASS()
class PORTIFOLIO_API UAsteroidWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TextInfo")
	void UpdateScore(int32 score);


	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "TextInfo")
	void UpdateLifeCount(int32 lifeCount);
	
};
