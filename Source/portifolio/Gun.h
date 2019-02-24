// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Collectable.h"
#include "Gun.generated.h"

UENUM()
enum class EGunType : uint8
{
	SlowGun,
	NormalGun,
	DoubleGun,
	FractalGun,
	EnumSize
};

UCLASS()
class PORTIFOLIO_API AGun : public ACollectable
{
	GENERATED_BODY()

private:

	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USoundBase* mpSoundFire;

	EGunType mGunType;

	bool mCanShoot;

	// Time between shootings
	float mShootCoolDown;

	// Will determine with what the projectile should collide
	FName mProjectileCollisionProfile;

public:

	// Sets default values for this actor's properties
	AGun();

	// Getters and setters
	UFUNCTION()
	EGunType GetGunType() const { return mGunType; }

	// Also sets material parameters
	UFUNCTION(BlueprintCallable)
	void SetGunType(const EGunType type);

	// Will attach this gun to pPawn with crRelativeT transform
	UFUNCTION(BlueprintCallable)
	void AttachToPawn(APawn* pPawn, const FTransform& crRelativeT);

	// Attach to pawn in pre-determined relative transform
	virtual void ApplyEffect(APawn* pAffectedPawn) override;

	// Spawn some projectile depending on gun type
	UFUNCTION()
	void Shoot();

	// After cooldown
	UFUNCTION()
	void EnableShooting();

};
