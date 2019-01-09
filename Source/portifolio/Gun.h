// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

UENUM()
enum class EGunType : uint8
{
	SlowGun,
	NormalGun,
	DoubleGun,
//	FractalGun,
	EnumSize
};

UCLASS()
class PORTIFOLIO_API AGun : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* mpMeshComponent;

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
	void SetType(const EGunType type);

	UFUNCTION()
	void Shoot();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
