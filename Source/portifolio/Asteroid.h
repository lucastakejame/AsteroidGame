// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
#include "Target.h"
#include "DamageInterface.h"
#include "Asteroid.generated.h"

class UStaticMeshComponent;

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
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
// members
	FAsteroidInfo mInfo;

// Methods
	// Sets default values for this actor's properties
	AAsteroid();

	// Called every frame
	virtual void Tick(float DeltaTime) override;


	virtual void Destroyed() override;

	UFUNCTION()
	void SetupAsteroid(FAsteroidInfo info, float hitPoints);

	// Damage Interface

	virtual void ReceiveDamage_Implementation(APawn* instigator, float damage) override;
};
