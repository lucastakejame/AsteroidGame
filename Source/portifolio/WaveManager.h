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
	// Player ship reference
	UPROPERTY()
	class AAsteroidShip* mpShipRef;

	UPROPERTY()
	TArray<class AAsteroid*> mAsteroidsArray;

	UPROPERTY()
	TArray<class AEnemyShip*> mEnemiesArray;

	UPROPERTY()
	TArray<class ACollectable*> mCollectablesArray;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAsteroid> mpAsteroidClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AEnemyShip> mpEnemyClass;
	
	UPROPERTY()
	UWorld* mpWorldRef;

	UPROPERTY()
	FTimerHandle mTimerHandleEnemySpawn;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	int32 mCurrentWave;	

// Delegates
	UPROPERTY(BluePrintAssignable)
	FWaveStartSignature mOnWaveStarting;

// Methods
	// Sets default values for this actor's properties
	AWaveManager();

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

	AAsteroid* SpawnAsteroid(const FTransform& t);

	void SpawnNAsteroids(int32 n, const std::function<FTransform(int32, int32)> getTransform);
};
