// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS(config=Game)
class AProjectile : public AActor
{
	GENERATED_BODY()

private:
	/** Sphere Mesh component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* mpMeshComponentProjectile;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* mpProjectileMovementComponent;

	UPROPERTY()
	float mDamage;

public:
	AProjectile();

	/** Function to handle the projectile hitting something */
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* pHitComp, AActor* pOtherActor, UPrimitiveComponent* pOtherComp, FVector normalImpulse, const FHitResult& crHit);

	FORCEINLINE UStaticMeshComponent* GetProjectileMesh() const { return mpMeshComponentProjectile; }

	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return mpProjectileMovementComponent; }

	FORCEINLINE float GetDamage() const { return mDamage; }

	void SetDamage(float damage) { mDamage = damage;  }

};

