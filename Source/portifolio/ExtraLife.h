// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Collectable.h"
#include "ExtraLife.generated.h"

/**
 * 
 */
UCLASS()
class PORTIFOLIO_API AExtraLife : public ACollectable
{
	GENERATED_BODY()

public:	
	// Add life to AsteroidShip
	virtual void ApplyEffect(APawn* pAffectedPawn) override;
	
};
