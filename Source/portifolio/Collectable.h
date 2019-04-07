// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Collectable.generated.h"

UENUM()
enum class ECollectableType : uint8
{
	ExtraLife,
	ControlBoost,
	Gun,
	EnumSize
};


UCLASS()
class PORTIFOLIO_API ACollectable : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(Category = Assets, VisibleDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* mpMeshComponent;

	ECollectableType mType;

public:
	// Sets default values for this actor's properties
	ACollectable();

	virtual void BeginPlay() override;

	UFUNCTION()
	void SetCollisionToCollectable();

	// Same thing as a abstract method
	UFUNCTION(BlueprintNativeEvent)
	void ApplyEffect(APawn* pAffectedPawn);
	virtual void ApplyEffect_Implementation(APawn* pAffectedPawn) {}

	// Getter and Setter
	ECollectableType GetType() { return mType; }

	// Setter
	UFUNCTION()
	void SetCollectableType(ECollectableType type)
	{
		mType = type;
	}

};