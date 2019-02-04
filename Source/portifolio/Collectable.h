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
	
	UPROPERTY(Category = Assets, VisibleDefaultsOnly, BlueprintReadWrite)
	class UMaterialInstanceDynamic* mpMID;

	ECollectableType mType;

public:
	// Sets default values for this actor's properties
	ACollectable();

	ECollectableType GetType() { return mType; }

	void SetCollectableType(ECollectableType type);

	static ACollectable* SpawnRandomCollectable(UWorld* world, const FTransform& t);
};
