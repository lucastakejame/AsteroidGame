// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageInterface.h"
#include "Asteroid.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotifyDeathSignature, AAsteroid*, destroyedAsteroidRef);

USTRUCT()
struct FAsteroidInfo
{
	GENERATED_BODY()

	FVector velocity;
	FVector angularVelocity;
	// Asteroid Size, Used to determine if should spawn smaller asteroids
	float scale;
	float hitPoints;
};


UCLASS()
class PORTIFOLIO_API AAsteroid : public AActor, public IDamageInterface
{
	GENERATED_BODY()
	
	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* mAsteroidMeshComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
// members
	// Sound to play each time we fire
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* mExplosionSound;
	
	FAsteroidInfo mInfo;

	FNotifyDeathSignature mNotifyDeathDelegate;

// Methods
	// Sets default values for this actor's properties
	AAsteroid();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnConstruction(const FTransform& Transform) override;
	
	UFUNCTION()
	void SetupAsteroid(FAsteroidInfo info);

	// Damage Interface
	
	virtual void ReceiveDamage_Implementation(APawn* instigator, float damage) override;
};
