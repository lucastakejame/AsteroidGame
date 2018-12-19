// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Asteroid.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class PORTIFOLIO_API AAsteroid : public AActor
{
	GENERATED_BODY()
	
	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* mAsteroidMeshComponent;

	UPROPERTY(Category = Movement, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* mProjectileMovementComponent;

public:	
	// Sets default values for this actor's properties
	AAsteroid();

	// Sound to play each time we fire
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* mExplosionSound;
	
	FVector mInitialVelocity;
	
	// Life fraction [0.,1.]
	float mLifeFraction;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION()
	void SetupAsteroid(FVector initialVelocity, float lifeFraction);

};
