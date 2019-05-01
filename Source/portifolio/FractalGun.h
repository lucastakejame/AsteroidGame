// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "FractalGun.generated.h"

/**
 * 
 */
UCLASS()
class PORTIFOLIO_API AFractalGun : public AGun
{
	GENERATED_BODY()
public:

	AFractalGun();
	
	virtual void Shoot_Implementation() override;
};
