// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "AsteroidGameMode.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNotificationSignature);

UCLASS()
class PORTIFOLIO_API AAsteroidGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AAsteroidGameMode();

    // For when Start game is pressed
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Event Dispatcher")
	FNotificationSignature mOnGameStarted;

    // For when last life is lost
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Event Dispatcher")
	FNotificationSignature mOnGameEnded;


};
