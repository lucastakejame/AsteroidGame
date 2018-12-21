// Fill out your copyright notice in the Description page of Project Settings.

#include "ScreenLimitManager.h"
#include "Classes/Engine/World.h"
#include "Engine/Public/EngineUtils.h"
#include "AsteroidShip.h"
#include "DrawDebugHelpers.h"

#include "DebugUtils.h"

// Sets default values
AScreenLimitManager::AScreenLimitManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mLimitWidth = 1024;
	mLimitHeight = 720;

}

void AScreenLimitManager::OnConstruction(const FTransform& transform)
{

	mWorld = GetWorld();
	
	if (mDrawDebugLimits)
	{
		DrawDebugBox(mWorld, FVector(0, 0, 0), FVector(mLimitHeight / 2, mLimitWidth / 2, 0), FColor(255, 0, 0, 255), false, 1, 0, 5);
	}
}


// Called when the game starts or when spawned
void AScreenLimitManager::BeginPlay()
{
	Super::BeginPlay();
	mWorld = GetWorld();

}

// Called every frame
void AScreenLimitManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (mWorld)
	{
		if (mDrawDebugLimits)
		{
			DrawDebugBox(mWorld, FVector(0, 0, 0), FVector(mLimitHeight / 2, mLimitWidth / 2, 0), FColor(255, 0, 0, 255), false, 0, 0, 5);
		}
		for (TActorIterator<AActor> actrItr(mWorld); actrItr; ++actrItr)
		{
			AActor* actor = *actrItr;


			if (actor && actor->IsRootComponentMovable() )
			{
				FVector oldLocation = actor->GetActorLocation();
				FVector newLocation = WrapLocation(actor->GetActorLocation(), FVector(-mLimitHeight / 2, -mLimitWidth / 2, 0), FVector(mLimitHeight / 2, mLimitWidth / 2, 0));
				actor->SetActorLocation(newLocation, false, (FHitResult *)nullptr, ETeleportType::TeleportPhysics);
			}
		}
	}

}

// corner0 has (minX,minY), corner1 has (maxX, maxY)
FVector AScreenLimitManager::WrapLocation(FVector location, FVector corner0, FVector corner1)
{
	FVector loc = location;
	FVector newLoc = loc;

	FVector ranges = FVector(abs((corner1 - corner0).X), abs((corner1 - corner0).Y), abs((corner1 - corner0).Z));

	if (loc.X < corner0.X)
	{
		newLoc.X = corner1.X - FMath::Fmod(abs(loc.X - corner0.X), ranges.X);
	}
	else if (loc.X > corner1.X)
	{
		newLoc.X = corner0.X + FMath::Fmod(abs(loc.X - corner1.X), ranges.X);
	}

	if (loc.Y < corner0.Y)
	{
		newLoc.Y = corner1.Y - FMath::Fmod(abs(loc.Y - corner0.Y), ranges.Y);
	}
	else if (loc.Y > corner1.Y)
	{
		newLoc.Y = corner0.Y + FMath::Fmod(abs(loc.Y - corner1.Y), ranges.Y);
	}

	return newLoc;
}

