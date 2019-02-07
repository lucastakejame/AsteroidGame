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

void AScreenLimitManager::OnConstruction(const FTransform& crT)
{
	if (mShouldDrawDebugLimits)
	{
		DrawDebugBox(GetWorld(), FVector(0, 0, 0), FVector(mLimitHeight / 2, mLimitWidth / 2, 0), FColor(255, 0, 0, 255), false, 1, 0, 5);
	}
}

void AScreenLimitManager::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	for (TActorIterator<AActor> itActor(GetWorld()); itActor; ++itActor)
	{
		AActor* pActor = *itActor;

		if (IsValid(pActor)
			&& pActor->ActorHasTag(FName("wrappable"))
			)
		{
			FVector oldLocation = pActor->GetActorLocation();
			FVector newLocation = WrapLocation(pActor->GetActorLocation(), FVector(-mLimitHeight / 2, -mLimitWidth / 2, 0), FVector(mLimitHeight / 2, mLimitWidth / 2, 0));
			pActor->SetActorLocation(newLocation, false, (FHitResult *)nullptr, ETeleportType::TeleportPhysics);
		}
	}
}

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

FVector AScreenLimitManager::GetRandomLocationInsideLimits() const
{

	FVector l;

	l.X = FMath::Lerp<float>(-mLimitHeight / 2, mLimitHeight / 2, FMath::FRand());
	l.Y = FMath::Lerp<float>(-mLimitWidth / 2, mLimitWidth / 2, FMath::FRand());
	l.Z = 0;

	return l;
}
