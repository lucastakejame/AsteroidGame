// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Target.h"
#include "DamageInterface.h"
#include "Asteroid.generated.h"

USTRUCT()
struct FAsteroidInfo
{
	GENERATED_BODY()

	FVector velocity;
	FVector angularVelocity;
	// Asteroid Size, Used to determine if should spawn smaller asteroids
	float scale;
};


UCLASS()
class PORTIFOLIO_API AAsteroid : public ATarget
{
	GENERATED_BODY()
private:
// members
	UPROPERTY(EditAnywhere)
	FAsteroidInfo mInfo;

public:
	UPROPERTY(EditAnywhere)
	TArray<UStaticMesh*> mMeshesPossible;

// Methods
	// Sets default values for this actor's properties
	AAsteroid();

	FAsteroidInfo GetInfo() const { return mInfo; }

	// update mInfo and applies velocities and scale
	UFUNCTION()
	void SetupAsteroid(const FAsteroidInfo& info, float hitPoints);

	// Damage Interface
	virtual void ReceiveDamage_Implementation(APawn* instigator, float damage) override;
};
