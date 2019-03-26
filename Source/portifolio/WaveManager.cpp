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
	PrimaryActorTick.bCanEverTick = false;

	mpClassAsteroid = AAsteroid::StaticClass();
	mpClassEnemy = AEnemyShip::StaticClass();

	mEnemySpawnInterval = 10.f;

	mChanceEnemyDropCollectable = .35;
	mChanceAsteroidDropCollectable = .15;
	mChanceSpawnEnemy = .5;
}

// Called when the game starts or when spawned
void AWaveManager::BeginPlay()
{
	Super::BeginPlay();
	AAsteroidGameMode* pGM = Cast<AAsteroidGameMode>(UGameplayStatics::GetGameMode(this));
	if (pGM)
	{
		pGM->mOnGameStarted.AddDynamic(this, &AWaveManager::StartWaves);
		pGM->mOnGameEnded.AddDynamic(this, &AWaveManager::ResetCurrentWave);
	}

	mpAsteroidShip = Cast<AAsteroidShip>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void AWaveManager::CleanTargets()
{
	// Destroy remaining asteroids and clean array
	for (auto* a : mArrayAsteroids)
	{
		if (IsValid(a))
		{
			a->Destroy();
		}
	}
	mArrayAsteroids.Empty();

	// Destroy remaining asteroids and clean array
	for (auto* e : mArrayEnemies)
	{
		if (IsValid(e))
		{
			e->Destroy();
		}
	}
	mArrayEnemies.Empty();
}

void AWaveManager::CleanCollectables()
{
	// Destroy remaining collectables and clean array
	for (auto* c : mArrayCollectables)
	{
		if (IsValid(c))
		{
			c->Destroy();
		}
	}
	mArrayEnemies.Empty();
}

void AWaveManager::StartWaves()
{
	CleanTargets();
	CleanCollectables();
	SpawnWave(mCurrentWave);
}

void AWaveManager::SpawnWave(int32 n)
{
	// current player ship location
	FVector shipLoc = (IsValid(mpAsteroidShip)) ? mpAsteroidShip->GetActorLocation() : FVector::ZeroVector;

	if (IsValid(GetWorld()))
	{
		switch (n)
		{
			case 0:
			{
				// Setting periodic enemy spawn tentative
				GetWorld()->GetTimerManager().SetTimer(mTimerHandleEnemySpawn, this, &AWaveManager::TryToSpawnEnemy, mEnemySpawnInterval, true);

				// This lambda generates transforms around the origin in a circumference, facing the center
				auto DistributeAround = [](int iCurrent, int loopSize) -> FTransform
				{
					// reference vector
					FVector ref = FVector(1,0,0);
					float angle = float(iCurrent)*360.f / loopSize;
					float radius = 500.f;

					FVector lv = radius * ref.RotateAngleAxis(angle, FVector(1, 0, 0));

					return FTransform(FRotator(0,angle+180.f,0), radius*ref.RotateAngleAxis(angle, FVector(0,0,1)));
				};

				SpawnNAsteroids(4, DistributeAround);

				// Setting up asteroid velocities and scale
				for (auto* itAsteroid : mArrayAsteroids)
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
				auto DistributeSides = [shipLoc](int iCurrent, int loopSize) -> FTransform
				{
					float alpha = float(iCurrent) / loopSize;
					FVector loc = FVector(
						FMath::Lerp(-500, 500, alpha),
						(iCurrent % 2 == 0)? 800: -800,
						0);

					loc += shipLoc;

					return FTransform(loc);
				};

				SpawnNAsteroids(6, DistributeSides);

				for (auto* itAsteroid : mArrayAsteroids)
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

				auto DistributeAroundRings = [shipLoc](int iCurrent, int loopSize) -> FTransform
				{
					FVector ref = FVector(1, 0, 0);
					float angle = (iCurrent < loopSize / 2.) ? 720.f*float(iCurrent) / loopSize: 720.f*float(iCurrent) / loopSize + 60.;
					float radius = (iCurrent < loopSize/2.) ? 500.f : 800.f;

					FVector lv = radius * ref.RotateAngleAxis(angle, FVector(1, 0, 0));

					return FTransform(FRotator(0, angle + 180.f, 0), shipLoc+radius*ref.RotateAngleAxis(angle, FVector(0, 0, 1)));
				};

				SpawnNAsteroids(10, DistributeAroundRings);

				for (auto* itAsteroid : mArrayAsteroids)
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
				// This lambda generates transforms on both top and bottom
				auto DistributeTopBottom = [shipLoc](int iCurrent, int loopSize) -> FTransform
				{
					float alpha = float(iCurrent) / loopSize;
					FVector loc = FVector(
						(iCurrent % 2 == 0) ? 800 : -800,
						FMath::Lerp(-1200, 1200, alpha),
						0);

					loc += shipLoc;

					return FTransform(loc);
				};

				SpawnNAsteroids(14, DistributeTopBottom);

				for (auto* itAsteroid : mArrayAsteroids)
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
				// This lambda generates transforms on a spiral around the player
				auto DistributeSpiral = [shipLoc](int iCurrent, int loopSize) -> FTransform
				{
					int numRings = 5;
					float percent = float(iCurrent) / loopSize;
					FVector ref = FVector(1, 0, 0);
					float angle = (360.f)*percent;
					float radius = 300.f + 600.f*(percent);

					FVector lv = radius * ref.RotateAngleAxis(angle, FVector(1, 0, 0));

					return FTransform(FRotator(0, angle + 180.f, 0), shipLoc + radius * ref.RotateAngleAxis(angle, FVector(0, 0, 1)));
				};

				SpawnNAsteroids(30, DistributeSpiral);

				for (auto* itAsteroid : mArrayAsteroids)
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

		mOnWaveStarting.Broadcast(n);
	}
}

void AWaveManager::ResetCurrentWave()
{
	mCurrentWave = 0;
}

void AWaveManager::HandleAsteroidDestruction(ATarget* pTarg)
{
	AAsteroid* pAsteroid = Cast<AAsteroid>(pTarg);

	if (IsValid(pAsteroid))
	{
		if (pAsteroid->GetInfo().scale > .6f)
		{
			FTransform t = pAsteroid->GetActorTransform();

			FAsteroidInfo info;
			info.scale = pAsteroid->GetInfo().scale*.8f;
			info.velocity = pAsteroid->GetInfo().velocity.RotateAngleAxis(-45, FVector(0, 0, 1))*1.5;
			info.angularVelocity = pAsteroid->GetInfo().angularVelocity.RotateAngleAxis(-45, FVector(1, 1, 1)) / 100;

			AAsteroid* pFragmentAsteroid = SpawnAsteroid(t);

			if (IsValid(pFragmentAsteroid))
			{
				pFragmentAsteroid->SetupAsteroid(info, pAsteroid->GetHitPoints()*.8f);
			}

			pFragmentAsteroid = SpawnAsteroid(t);

			info.velocity = pAsteroid->GetInfo().velocity.RotateAngleAxis(45, FVector(0, 0, 1))*1.5;
			info.angularVelocity = pAsteroid->GetInfo().angularVelocity.RotateAngleAxis(45, FVector(1, 1, 1)) / 100;

			if (IsValid(pFragmentAsteroid))
			{
				pFragmentAsteroid->SetupAsteroid(info, pAsteroid->GetHitPoints()*.8f);
			}
		}
		else
		{
			// Chance of spawning a collectable
			if (IsValid(GetWorld()) && FMath::FRand() <= mChanceAsteroidDropCollectable)
			{
				FTransform t = FTransform( pAsteroid->GetActorLocation());

				ACollectable* pC = SpawnRandomCollectable(t);

				if (IsValid(pC)) mArrayCollectables.Add(pC);
			}
		}
		pAsteroid->Destroy();

		mArrayAsteroids.Remove(pAsteroid);

		if (mArrayAsteroids.Num() == 0) SpawnWave(++mCurrentWave);

	}
}

void AWaveManager::HandleEnemyDestruction(ATarget* pTarg)
{
	ATarget* pEnemy = Cast<ATarget>(pTarg);
	if (IsValid(pEnemy))
	{
		pEnemy->Destroy();
		mArrayEnemies.Remove(pEnemy);

		// Chance of spawning a collectable
		if (IsValid(GetWorld()) && FMath::FRand() <= mChanceEnemyDropCollectable)
		{
			ACollectable* pC = SpawnRandomCollectable(FTransform(pTarg->GetActorLocation()) );

			if(IsValid(pC)) mArrayCollectables.Add(pC);
		}
	}
}

void AWaveManager::TryToSpawnEnemy()
{
	// 50% chance of spawning an enemy ship, but it decreases if there is enemies already
	if (IsValid(GetWorld()) && FMath::FRand() <= mChanceSpawnEnemy - mArrayEnemies.Num()/4. )
	{
		FTransform t;
		t.SetLocation(FVector(
			FMath::Lerp(-1000,1000, FMath::FRand()),
			FMath::Lerp(-1000, 1000, FMath::FRand()),
			0 ));

		FActorSpawnParameters sp;
		sp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;


		mpClassEnemy = (IsValid(mpClassEnemy)) ? mpClassEnemy : AEnemyShip::StaticClass();

		ATarget* pEnemy = GetWorld()->SpawnActor<ATarget>(mpClassEnemy, t, sp);

		pEnemy->SetHitPoints(50 + mCurrentWave * 25);

		pEnemy->mOnDeath.AddDynamic(this, &AWaveManager::HandleEnemyDestruction);

		mArrayEnemies.Add(pEnemy);
	}
}

AAsteroid* AWaveManager::SpawnAsteroid(const FTransform& crT)
{
	FActorSpawnParameters sp;
	sp.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	mpClassAsteroid = ( IsValid(mpClassAsteroid))? mpClassAsteroid : AAsteroid::StaticClass();

	AAsteroid* pA = GetWorld()->SpawnActor<AAsteroid>(mpClassAsteroid, crT, sp);

	mArrayAsteroids.Add(pA);

	pA->mOnDeath.AddDynamic(this, &AWaveManager::HandleAsteroidDestruction);

	return pA;
}

void AWaveManager::SpawnNAsteroids(int32 n, const std::function<FTransform(int32 iCurrent, int32 loopSize)> GetTransform)
{
	for (int32 i = 0; i < n; i++)
	{
		FTransform t;

		t = GetTransform(i, n);

		SpawnAsteroid(t);
	}
}

ACollectable* AWaveManager::SpawnRandomCollectable(const FTransform& crT)
{
	ACollectable* pResult = nullptr;

	TSubclassOf<ACollectable> pSpawnClass;

	// Sums chances weights to get random max value
	float chanceSum = 0;
	for (auto p : mMapCollectChance){ chanceSum += p.Value;}

	float rand = FMath::FRandRange(0, chanceSum);

	// Check which class the random value picked
	chanceSum = 0;
	for (auto p : mMapCollectChance)
	{
		chanceSum += p.Value;
		if (rand <= chanceSum)
		{
			pSpawnClass = p.Key;
			break;
		}
	}
	
	if (IsValid(pSpawnClass))
	{
		pResult = GetWorld()->SpawnActor<ACollectable>(pSpawnClass, crT);
	}

	return pResult;
}