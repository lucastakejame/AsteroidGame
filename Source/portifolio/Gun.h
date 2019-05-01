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

protected:

	// Time between shootings
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float mShootCoolDown;

	// Will determine with what the projectile should collide
	FName mProjectileCollisionProfile;

	// Keeps track if gun is on cooldown or not
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool mCanShoot;

	// Gun type identifier
	EGunType mGunType;

	// Marks last time a shoot was successful, used when checking if its ok to shoot again.
	float mLastTimeShot = 0;

public:

	// Sets default values for this actor's properties
	AGun();

	// Getters and setters
	UFUNCTION()
	EGunType GetGunType() const { return mGunType; }

	// Will attach this gun to pPawn with crRelativeT transform
	UFUNCTION(BlueprintCallable)
	void AttachToPawn(APawn* pPawn, const FTransform& crRelativeT);

	// Attach to pawn in pre-determined relative transform
	void ApplyEffect_Implementation(APawn* pAffectedPawn);

	// Spawn some projectile depending on gun type and if not on cooldown
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Shoot();
	virtual void Shoot_Implementation();

	// After cooldown
	UFUNCTION()
	void EnableShooting();

};
