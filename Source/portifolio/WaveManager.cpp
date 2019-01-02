// Fill out your copyright notice in the Description page of Project Settings.

#include "WaveManager.h"
#include "AsteroidGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Asteroid.h"
#include "Containers/Array.h"
#include "SharedPointer.h"
#include "Engine/World.h"
#include "Target.h"

#include "DebugUtils.h"


// Sets default values
AWaveManager::AWaveManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mAsteroidsArray = TArray<AAsteroid*>();
}

// Called when the game starts or when spawned
void AWaveManager::BeginPlay()
{
	Super::BeginPlay();
	AAsteroidGameMode* gm = Cast<AAsteroidGameMode>(UGameplayStatics::GetGameMode(this));

	if (gm)
	{
		gm->mNotifyGameStartDelegate.AddDynamic(this, &AWaveManager::StartWaves);
	}
}

// Called every frame
void AWaveManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWaveManager::SpawnWave(int n)
{
	UWorld* w = this->GetWorld();
	switch (n)
	{
	case 0:
	{
		for (int i = 0; i < 4; i++)
		{
			FTransform t = GetActorTransform();
				
			t.SetLocation(FVector(
				(FMath::RandBool() ? 1 : -1)*FMath::Lerp<float, float>(300., 500., FMath::FRand()),
				(FMath::RandBool() ? 1 : -1)*FMath::Lerp<float, float>(300., 500., FMath::FRand()), 0));

			//AAsteroid* piece0 = w->SpawnActorDeferred<AAsteroid>(AAsteroid::StaticClass(), t);
			AAsteroid* piece0 = w->SpawnActor<AAsteroid>(AAsteroid::StaticClass(), t);

			FAsteroidInfo info;

			// cross product so velocity is perpendicular to radius
			info.velocity = t.GetLocation().GetSafeNormal()^FVector(0, 0, 1)*50.;
			info.angularVelocity = FVector(1, 0, 0);
			info.scale = 1.f;

			piece0->SetupAsteroid(info, 100);

			piece0->mNotifyDeathDelegate.AddDynamic(this, &AWaveManager::DestroyAsteroidAndTryToSpawnSmaller);

			mAsteroidsArray.Add(piece0);
		}
	}
		break;

	default:
		break;
	}
}

void AWaveManager::StartWaves()
{
	// Destroy remaining asteroids and clean array
	for(auto* ast : mAsteroidsArray)
	{
		if (IsValid(ast))
		{
			ast->Destroy();
		}
	}
	mAsteroidsArray.Empty();

	SpawnWave(0);
}

void AWaveManager::DestroyAsteroidAndTryToSpawnSmaller(ATarget* targ)
{	
	AAsteroid* asteroid = Cast<AAsteroid>(targ);

	if (IsValid(asteroid))
	{
		if (asteroid->mInfo.scale > .6f)
		{
			UWorld* world = asteroid->GetWorld();

			FTransform thisTransform = asteroid->GetActorTransform();

			AAsteroid* piece0 = world->SpawnActorDeferred<AAsteroid>(asteroid->GetClass(), thisTransform);
			AAsteroid* piece1 = world->SpawnActorDeferred<AAsteroid>(asteroid->GetClass(), thisTransform);

			FAsteroidInfo info;
			info.scale = asteroid->mInfo.scale*.8f;
			info.velocity = asteroid->mInfo.velocity.RotateAngleAxis(-45, FVector(0, 0, 1))*1.5;
			info.angularVelocity = asteroid->mInfo.angularVelocity.RotateAngleAxis(-45, FVector(1, 1, 1)) / 100;

			piece0->SetupAsteroid(info, asteroid->mHitPoints*.8f);

			info.velocity = asteroid->mInfo.velocity.RotateAngleAxis(45, FVector(0, 0, 1))*1.5;
			info.angularVelocity = asteroid->mInfo.angularVelocity.RotateAngleAxis(45, FVector(1, 1, 1)) / 100;

			piece1->SetupAsteroid(info, asteroid->mHitPoints*.8f);

			piece0->FinishSpawning(thisTransform);
			piece1->FinishSpawning(thisTransform);

			piece0->mNotifyDeathDelegate.AddDynamic(this, &AWaveManager::DestroyAsteroidAndTryToSpawnSmaller);
			piece1->mNotifyDeathDelegate.AddDynamic(this, &AWaveManager::DestroyAsteroidAndTryToSpawnSmaller);

			mAsteroidsArray.Add(piece0);
			mAsteroidsArray.Add(piece1);
		}

		mAsteroidsArray.Remove(asteroid);

		asteroid->Destroy();
	}
}