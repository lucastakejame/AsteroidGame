// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ScreenLimitManager.generated.h"

UCLASS()
class PORTIFOLIO_API AScreenLimitManager : public AActor
{
	GENERATED_BODY()

	UWorld* mWorld;
	
	
public:	
	// Sets default values for this actor's properties
	AScreenLimitManager();

	UPROPERTY(EditAnywhere)
	float mLimitWidth;


	UPROPERTY(EditAnywhere)
	float mLimitHeight;

	UPROPERTY(EditAnywhere)
	bool mDrawDebugLimits;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(const FTransform& transform) override;

	FVector WrapLocation(FVector location, FVector corner0, FVector corner1);

};
