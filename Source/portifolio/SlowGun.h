// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "SlowGun.generated.h"

/**
 * 
 */
UCLASS()
class PORTIFOLIO_API ASlowGun : public AGun
{
	GENERATED_BODY()
	
	ASlowGun();

	virtual void Shoot_Implementation() override;
};
