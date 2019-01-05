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

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Event Dispatcher")
	FNotificationSignature mNotifyGameStartDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Event Dispatcher")
	FNotificationSignature mNotifyGameOverDelegate;


};
