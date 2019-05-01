// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "DoubleGun.generated.h"

/**
 * 
 */
UCLASS()
class PORTIFOLIO_API ADoubleGun : public AGun
{
	GENERATED_BODY()

	ADoubleGun();

	virtual void Shoot_Implementation() override;
};
