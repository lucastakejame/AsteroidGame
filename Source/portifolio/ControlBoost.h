// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Collectable.h"
#include "ControlBoost.generated.h"

/**
 * 
 */
UCLASS()
class PORTIFOLIO_API AControlBoost : public ACollectable
{
	GENERATED_BODY()
private:
	
	UPROPERTY(Category = Boost, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float mSpeedBoost = 2.;

	UPROPERTY(Category = Boost, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float mRotationalBoost = 1.4;

public:

	// Boost asteroidShip's Velocity and rotationVelocity
	virtual void ApplyEffect(APawn* pAffectedPawn) override;
};
