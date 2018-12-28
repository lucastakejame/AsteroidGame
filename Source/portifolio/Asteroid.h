// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageInterface.h"
#include "Asteroid.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class PORTIFOLIO_API AAsteroid : public AActor, public IDamageInterface
{
	GENERATED_BODY()
	
	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* mAsteroidMeshComponent;

public:	
	// Sets default values for this actor's properties
	AAsteroid();

	// Sound to play each time we fire
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* mExplosionSound;
	
	FVector mInitialVelocity;
	FVector mInitialAngularVelocity;
		
	// Asteroid Scale
	float mScale;

	float mHitPoints;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnConstruction(const FTransform& Transform) override;
	
	UFUNCTION()
	void SetupAsteroid(FVector initialVelocity, FVector mInitialAngularVelocity, float scale, float hitPoints);

	// Damage Interface
	
	virtual void ReceiveDamage_Implementation(APawn* instigator, float damage) override;
};
