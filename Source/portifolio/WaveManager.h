// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <functional>
#include "GameFramework/Actor.h"
#include "WaveManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaveStartSignature, int32, waveBeingStarted);

UCLASS()
class PORTIFOLIO_API AWaveManager : public AActor
{
	GENERATED_BODY()


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
// Members
	class AAsteroidShip* mpShipRef;

	class TArray<class AAsteroid*> mAsteroidsArray;

	TArray<class AEnemyShip*> mEnemiesArray;

	TArray<class ACollectable*> mCollectablesArray;

	UWorld* mpWorldRef;

	FTimerHandle mTimerHandleEnemySpawn;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	int32 mCurrentWave;	

// Delegates
	UPROPERTY(BluePrintAssignable)
	FWaveStartSignature mWaveStartDelegate;

// Methods
	// Sets default values for this actor's properties
	AWaveManager();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnWave(int32 n);

	UFUNCTION()
	void StartWaves();

	UFUNCTION()
	void ResetCurrentWave();
	
	UFUNCTION()
	void TryToSpawnEnemy();

	void CleanTargets();

	void CleanCollectables();
	
	UFUNCTION()
	void HandleAsteroidDestruction(class ATarget* targ);

	UFUNCTION()
	void HandleEnemyDestruction(class ATarget* targ);

	void SpawnNAsteroids(int32 n, const std::function<FTransform(int32, int32)> getTransform);
};
