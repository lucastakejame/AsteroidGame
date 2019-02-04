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
	class UStaticMeshComponent* mpProjectileMesh;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* mpProjectileMovement;

	UPROPERTY()
	float mDamage;

public:
	AProjectile();

	/** Function to handle the projectile hitting something */
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FORCEINLINE UStaticMeshComponent* GetProjectileMesh() const { return mpProjectileMesh; }

	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return mpProjectileMovement; }

	FORCEINLINE float GetDamage() const { return mDamage; }
	
	void SetDamage(float damage) { mDamage = damage;  }

};

