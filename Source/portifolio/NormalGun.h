// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "NormalGun.generated.h"

/**
 * 
 */
UCLASS()
class PORTIFOLIO_API ANormalGun : public AGun
{
	GENERATED_BODY()
	
	ANormalGun();

	virtual void Shoot_Implementation() override;
};
