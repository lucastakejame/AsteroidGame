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
private:
// Members
	// Player ship
	UPROPERTY()
	class AAsteroidShip* mpAsteroidShip;

	UPROPERTY()
	TArray<class AAsteroid*> mArrayAsteroids;

	UPROPERTY()
	TArray<class ATarget*> mArrayEnemies;

	UPROPERTY()
	TArray<class ACollectable*> mArrayCollectables;

	// Classes of spawned stuff
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AAsteroid> mpClassAsteroid;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ATarget> mpClassEnemy;

	// maps which collectable class is spawnable and the chance weight
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap< TSubclassOf<class ACollectable>, float> mMapCollectChance;

	UPROPERTY()
	FTimerHandle mTimerHandleEnemySpawn;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 mCurrentWave;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float mEnemySpawnInterval;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float mChanceSpawnEnemy;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float mChanceEnemyDropCollectable;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float mChanceAsteroidDropCollectable;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
// Delegates
	UPROPERTY(BluePrintAssignable)
	FWaveStartSignature mOnWaveStarting;

// Methods
	// Sets default values for this actor's properties
	AWaveManager();

	// Clean arrays
	UFUNCTION(BlueprintCallable)
	void CleanTargets();

	UFUNCTION(BlueprintCallable)
	void CleanCollectables();

	// Start game
	UFUNCTION(BlueprintCallable)
	void StartWaves();

	// spawn wave n
	UFUNCTION(BlueprintCallable)
	void SpawnWave(int32 n);

	// Sets currentWave to 0
	UFUNCTION(BlueprintCallable)
	void ResetCurrentWave();

	// spawn 2 smaller asteroids in place or roll dice to place collectable
	UFUNCTION()
	void HandleAsteroidDestruction(class ATarget* pTarg);

	// roll dice to place collectable
	UFUNCTION()
	void HandleEnemyDestruction(class ATarget* pTarg);

	// Spawning methods
	// Roll a dice and see if it should spawn an enemy
	// If succeeded: Spawn enemy at random location, add it to array and make binding
	UFUNCTION(BlueprintCallable)
	void TryToSpawnEnemy();

	// Spawn asteroid at transform, add it to array and make binding
	UFUNCTION(BlueprintCallable)
	AAsteroid* SpawnAsteroid(const FTransform& crT);

	// Lambda function receives current index in the loop and size of loop as parameters to get the spawned asteroid transform
	void SpawnNAsteroids(int32 n, const std::function<FTransform(int32 iCurrent, int32 loopSize)> GetTransform);

	// Spawn a collectable of random sort
	UFUNCTION(BlueprintCallable)
	ACollectable* SpawnRandomCollectable(const FTransform& crT);

	// Spawn a collectable
	UFUNCTION(BlueprintCallable)
	ACollectable* SpawnCollectable(TSubclassOf<ACollectable> pSpawnClass, const FTransform& crT);
};
