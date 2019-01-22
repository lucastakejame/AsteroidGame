 // Fill out your copyright notice in the Description page of Project Settings.

#include "WaveManager.h"
#include "AsteroidGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Asteroid.h"
#include "Containers/Array.h"
#include "SharedPointer.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Target.h"
#include "EnemyShip.h"
#include "AsteroidShip.h"
#include "Collectable.h"
#include "Gun.h"

#include "DebugUtils.h"


// Sets default values
AWaveManager::AWaveManager() : mCurrentWave(0)
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
		gm->mNotifyGameOverDelegate.AddDynamic(this, &AWaveManager::ResetCurrentWave);
	}

	mpWorldRef = this->GetWorld();

	mpShipRef = Cast<AAsteroidShip>(UGameplayStatics::GetPlayerPawn(this, 0));
}

// Called every frame
void AWaveManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWaveManager::SpawnWave(int32 n)
{
	// current player ship location
	FVector shipLoc = (IsValid(mpShipRef)) ? mpShipRef->GetActorLocation() : FVector::ZeroVector;

	if (IsValid(mpWorldRef))
	{
		switch (n)
		{
			case 0:
			{
				// This maybe too fast enemy spawning
				mpWorldRef->GetTimerManager().SetTimer(mTimerHandleEnemySpawn, this, &AWaveManager::TryToSpawnEnemy, 10., true);

				// This lambda generates transforms around the origin in a circumference, facing the center 
				auto distributeAround = [](int idxNow, int idxMax) -> FTransform 
				{
					FVector ref = FVector(1,0,0);
					float angle = float(idxNow)*360.f / idxMax;
					float radius = 500.f;
					
					FVector lv = radius * ref.RotateAngleAxis(angle, FVector(1, 0, 0));

					return FTransform(FRotator(0,angle+180.f,0), radius*ref.RotateAngleAxis(angle, FVector(0,0,1)));
				};

				SpawnNAsteroids(4, distributeAround);

				for (auto* itAsteroid : mAsteroidsArray)
				{
					FAsteroidInfo info;

					// cross product so velocity is perpendicular to radius
					info.velocity = itAsteroid->GetActorLocation().GetSafeNormal()^FVector(0, 0, 1)*50.;
					info.angularVelocity = FVector(1, 0, 0);
					info.scale = .8f;

					itAsteroid->SetupAsteroid(info, 100);
				}
			}
			break;

			case 1:
			{
				// This lambda generates transforms on both player sides
				auto distributeSides = [shipLoc](int idxNow, int idxMax) -> FTransform
				{
					float alpha = float(idxNow) / idxMax;
					FVector loc = FVector(
						FMath::Lerp(-500, 500, alpha),
						(idxNow % 2 == 0)? 800: -800,
						0);

					loc += shipLoc;

					return FTransform(loc);
				};

				SpawnNAsteroids(6, distributeSides);

				for (auto* itAsteroid : mAsteroidsArray)
				{
					FAsteroidInfo info;
					
					// velocity aims player
					info.velocity = -200.*(itAsteroid->GetActorLocation() - shipLoc).GetSafeNormal();
					info.angularVelocity = FVector(1, 0, 0);
					info.scale = 1.f;

					itAsteroid->SetupAsteroid(info, 100);
				}
			}
			break;

			case 2:
			{

				auto distributeAroundRings = [shipLoc](int idxNow, int idxMax) -> FTransform
				{
					FVector ref = FVector(1, 0, 0);
					float angle = (idxNow < idxMax / 2.) ? 720.f*float(idxNow) / idxMax: 720.f*float(idxNow) / idxMax + 60.;
					float radius = (idxNow < idxMax/2.) ? 500.f : 800.f;

					FVector lv = radius * ref.RotateAngleAxis(angle, FVector(1, 0, 0));

					return FTransform(FRotator(0, angle + 180.f, 0), shipLoc+radius*ref.RotateAngleAxis(angle, FVector(0, 0, 1)));
				};

				SpawnNAsteroids(10, distributeAroundRings);

				for (auto* itAsteroid : mAsteroidsArray)
				{
					FAsteroidInfo info;

					// cross product so velocity is perpendicular to radius
					info.velocity = itAsteroid->GetActorLocation().GetSafeNormal() ^ FVector(0, 0, 1)*50.;
					info.angularVelocity = FVector(0, 1, 0);
					info.scale = 1.f;

					itAsteroid->SetupAsteroid(info, 100);
				}
			}
			break;

			case 3:
			{
				// This lambda generates transforms on both player sides
				auto distributeTopBottom = [shipLoc](int idxNow, int idxMax) -> FTransform
				{
					float alpha = float(idxNow) / idxMax;
					FVector loc = FVector(
						(idxNow % 2 == 0) ? 800 : -800,
						FMath::Lerp(-1200, 1200, alpha),
						0);

					loc += shipLoc;

					return FTransform(loc);
				};

				SpawnNAsteroids(14, distributeTopBottom);

				for (auto* itAsteroid : mAsteroidsArray)
				{
					FAsteroidInfo info;

					// velocity aims player
					info.velocity = -200.*(itAsteroid->GetActorLocation() - shipLoc).GetSafeNormal();
					info.angularVelocity = FVector(1, 0, 0);
					info.scale = 1.f;

					itAsteroid->SetupAsteroid(info, 100);
				}
			}
			break;

			case 4:
			{
				// This lambda generates transforms on both player sides
				auto distributeAllOver = [shipLoc](int idxNow, int idxMax) -> FTransform
				{
					int numRings = 5;
					float percent = float(idxNow) / idxMax;
					FVector ref = FVector(1, 0, 0);
					float angle = (360.f)*percent;
					float radius = 300.f + 600.f*(percent);

					FVector lv = radius * ref.RotateAngleAxis(angle, FVector(1, 0, 0));

					return FTransform(FRotator(0, angle + 180.f, 0), shipLoc + radius * ref.RotateAngleAxis(angle, FVector(0, 0, 1)));
				};

				SpawnNAsteroids(30, distributeAllOver);

				for (auto* itAsteroid : mAsteroidsArray)
				{
					FAsteroidInfo info;

					// velocity aims player
					info.velocity = -100.*(itAsteroid->GetActorLocation() - shipLoc).GetSafeNormal();
					info.angularVelocity = FVector(1, 0, 0);
					info.scale = 1.f;

					itAsteroid->SetupAsteroid(info, 100);
				}
			}
			break;
			
			default:
			break;

		}

		mWaveStartDelegate.Broadcast(n);
	}

}

void AWaveManager::StartWaves()
{
	CleanTargets();
	CleanCollectables();
	SpawnWave(mCurrentWave);
}

void AWaveManager::ResetCurrentWave()
{
	mCurrentWave = 0;
}

void AWaveManager::TryToSpawnEnemy()
{
	// 50% chance of spawning an enemy ship, but it decreases if there is enemies already
	if (IsValid(mpWorldRef) && FMath::FRand() > .5 + mEnemiesArray.Num()/4. )
	{
		FTransform t;
		t.SetLocation(FVector(
			FMath::Lerp(-1000,1000, FMath::FRand()),
			FMath::Lerp(-1000, 1000, FMath::FRand()),
			0 ));

		FActorSpawnParameters sp; 
		sp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		auto* enemy = mpWorldRef->SpawnActor<AEnemyShip>(AEnemyShip::StaticClass(), t, sp);

		enemy->mHitPoints = 50 + mCurrentWave * 25;

		enemy->mNotifyDeathDelegate.AddDynamic(this, &AWaveManager::HandleEnemyDestruction);

		mEnemiesArray.Add(enemy);
	}
}


void AWaveManager::CleanTargets()
{
	// Destroy remaining asteroids and clean array
	for (auto* a : mAsteroidsArray)
	{
		if (IsValid(a))
		{
			a->Destroy();
		}
	}
	mAsteroidsArray.Empty();

	// Destroy remaining asteroids and clean array
	for (auto* e : mEnemiesArray)
	{
		if (IsValid(e))
		{
			e->Destroy();
		}
	}
	mEnemiesArray.Empty();
}

void AWaveManager::CleanCollectables()
{
	// Destroy remaining collectables and clean array
	for (auto* c : mCollectablesArray)
	{
		if (IsValid(c))
		{
			c->Destroy();
		}
	}
	mEnemiesArray.Empty();
}



void AWaveManager::HandleAsteroidDestruction(ATarget* targ)
{	
	AAsteroid* asteroid = Cast<AAsteroid>(targ);

	if (IsValid(asteroid))
	{
		if (asteroid->mInfo.scale > .6f)
		{
			FTransform thisTransform = asteroid->GetActorTransform();

			FActorSpawnParameters sp;
			sp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AAsteroid* piece0 = mpWorldRef->SpawnActor<AAsteroid>(asteroid->GetClass(), thisTransform, sp);
			AAsteroid* piece1 = mpWorldRef->SpawnActor<AAsteroid>(asteroid->GetClass(), thisTransform, sp);

			FAsteroidInfo info;
			info.scale = asteroid->mInfo.scale*.8f;
			info.velocity = asteroid->mInfo.velocity.RotateAngleAxis(-45, FVector(0, 0, 1))*1.5;
			info.angularVelocity = asteroid->mInfo.angularVelocity.RotateAngleAxis(-45, FVector(1, 1, 1)) / 100;

			if (IsValid(piece0))
			{
				piece0->SetupAsteroid(info, asteroid->mHitPoints*.8f);
			}

			info.velocity = asteroid->mInfo.velocity.RotateAngleAxis(45, FVector(0, 0, 1))*1.5;
			info.angularVelocity = asteroid->mInfo.angularVelocity.RotateAngleAxis(45, FVector(1, 1, 1)) / 100;

			if (IsValid(piece0))
			{
				piece1->SetupAsteroid(info, asteroid->mHitPoints*.8f);
			}

			piece0->mNotifyDeathDelegate.AddDynamic(this, &AWaveManager::HandleAsteroidDestruction);
			piece1->mNotifyDeathDelegate.AddDynamic(this, &AWaveManager::HandleAsteroidDestruction);

			mAsteroidsArray.Add(piece0);
			mAsteroidsArray.Add(piece1);
		}
		else
		{
			// Change of spawning a collectable
			if (IsValid(mpWorldRef) && FMath::FRand() <= .15)
			{
				FTransform t = FTransform(asteroid->GetActorRotation(), asteroid->GetActorLocation());

				ACollectable* c = ACollectable::SpawnRandomCollectable(mpWorldRef, t);

				if (IsValid(c)) mCollectablesArray.Add(c);
			}
		}
		asteroid->Destroy();

		mAsteroidsArray.Remove(asteroid);
		
		if (mAsteroidsArray.Num() == 0) SpawnWave(++mCurrentWave);
		
	}
}

void AWaveManager::HandleEnemyDestruction(ATarget* targ)
{
	AEnemyShip* enemy = Cast<AEnemyShip>(targ);
	if (IsValid(enemy))
	{
		enemy->Destroy();
		mEnemiesArray.Remove(enemy);


		// Change of spawning a collectable
		if (IsValid(mpWorldRef) && FMath::FRand() <= .35)
		{
			ACollectable* c = ACollectable::SpawnRandomCollectable(mpWorldRef, targ->GetActorTransform());

			if(IsValid(c)) mCollectablesArray.Add(c);
		}
	}
}

void AWaveManager::SpawnNAsteroids(int32 n, const std::function<FTransform(int32, int32)> getTransform)
{
	FTransform t;
	t.SetLocation(FVector::ZeroVector);
	t.SetRotation(FQuat(FRotator::ZeroRotator));
	t.SetScale3D(FVector(1,1,1));

	
	FActorSpawnParameters sp;
	sp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < n; i++)
	{

		t = getTransform(i, n);

		AAsteroid* a = mpWorldRef->SpawnActor<AAsteroid>(AAsteroid::StaticClass(),t, sp);

		mAsteroidsArray.Add(a);

		a->mNotifyDeathDelegate.AddDynamic(this, &AWaveManager::HandleAsteroidDestruction);

	}


}