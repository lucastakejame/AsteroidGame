// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Target.h"
#include "EnemyShip.generated.h"

/**
 * 
 */
UCLASS()
class PORTIFOLIO_API AEnemyShip : public ATarget
{
	GENERATED_BODY()

public:
// Members
	FVector mMovingDirection;

	class AGun* mpGun = nullptr;

// Methods
	AEnemyShip();

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	
	void ChangeMovingDirection();

	void Shoot();
};
