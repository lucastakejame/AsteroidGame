// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveManager.generated.h"


UCLASS()
class PORTIFOLIO_API AWaveManager : public AActor
{
	GENERATED_BODY()

	class AAsteroidShip* mPlayerRef; 

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
// Members

	class TArray<class AAsteroid*> mAsteroidsArray;

// Methods
	// Sets default values for this actor's properties
	AWaveManager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnWave(int n);

	UFUNCTION()
	void StartWaves();

	UFUNCTION()
	void DestroyAsteroidAndTryToSpawnSmaller(class ATarget* targ);
};
