// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "DamageInterface.h"
#include "Target.generated.h"

class UStaticMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotifyDeathSignature, ATarget*, destroyedTargetRef);

UCLASS()
class PORTIFOLIO_API ATarget : public APawn, public IDamageInterface
{
	GENERATED_BODY()
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
// Components
	// The mesh component
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* mMeshComponent;

// Members
	// Sound to play each time we fire
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* mExplosionSound;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float mHitPoints;

	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	int32 mScoreValue;

	FNotifyDeathSignature mNotifyDeathDelegate;

// Methods
	// Sets default values for this actor's properties
	ATarget();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

// Damage Interface

	virtual void ReceiveDamage_Implementation(APawn* instigator, float damage) override;
};
