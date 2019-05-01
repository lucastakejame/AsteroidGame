// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Target.h"
#include "DamageInterface.h"
#include "Asteroid.generated.h"

// This will be used to pass information for asteroid fragments
USTRUCT(BlueprintType)
struct FAsteroidInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector velocity;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector angularVelocity;

	// Asteroid Size, Used to determine if should spawn smaller asteroids
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float scale;
};


UCLASS(BlueprintType)
class PORTIFOLIO_API AAsteroid : public ATarget
{
	GENERATED_BODY()
private:
// members
	UPROPERTY(EditAnywhere)
	FAsteroidInfo mInfo;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<UStaticMesh*> mMeshesPossible;

public:

// Methods
	// Sets default values for this actor's properties
	AAsteroid();

	// Getter
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FORCEINLINE FAsteroidInfo GetInfo() const { return mInfo; }

	// update mInfo and applies velocities and scale
	UFUNCTION(BlueprintCallable)
	void SetupAsteroid(const FAsteroidInfo& crInfo, float hitPoints);

	// Damage Interface
	virtual void ReceiveDamage_Implementation(APawn* pInstigator, float damage) override;
};
