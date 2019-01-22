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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:


	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* mpFireSound;

	EGunType mGunType;

	UWorld* mpWorld;

	bool mCanShoot;

	float mShootPeriod;

	FName mProjectileCollisionProfile;


	// Sets default values for this actor's properties
	AGun();

	UFUNCTION()
	void EnableShooting();

	UFUNCTION()
	void AttachToPawn(APawn* pawn, FTransform relativeT);

	UFUNCTION(BlueprintCallable)
	void SetGunType(const EGunType type);

	UFUNCTION()
	void Shoot();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
