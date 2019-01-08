 // Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"
#include "portifolioProjectile.h"
#include "Engine/World.h"
#include "ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

#include "DebugUtils.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Fetching Assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<USoundBase> fireSoundAsset(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> materialAsset(TEXT("/Game/Asteroids/Art/Gun/M_Gun.M_Gun"));

	mpMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
	RootComponent = mpMeshComponent;

	if (meshAsset.Succeeded())
	{
		mpMeshComponent->SetStaticMesh(meshAsset.Object);
		if (materialAsset.Succeeded())
		{
			mpMeshComponent->SetMaterial(0, materialAsset.Object);
		}
	}
	if (fireSoundAsset.Succeeded()) mpFireSound = fireSoundAsset.Object;

	mpMeshComponent->SetCollisionProfileName("OverlapAll");
	mpMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AGun::OnOverlap);
	mpMeshComponent->SetGenerateOverlapEvents(true);

	mCanShoot = true;
	SetType(EGunType::NormalGun);
}


void AGun::SetType(const EGunType t)
{
	mGunType = t;
	switch (t)
	{
		case EGunType::NormalGun:
		{
			mShootPeriod = .1;
		}
		break;
		case EGunType::EnemyGun:
		{
			mShootPeriod = .5;
		}
		break;
	}
}


void AGun::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
}

void AGun::AttachToPawn(APawn* pawn, FTransform relativeT)
{
	SetOwner(pawn);
	mpMeshComponent->SetHiddenInGame(true);
	mpMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	AttachToActor(pawn, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SetActorRelativeTransform(relativeT);

}

void AGun::Shoot()
{
	switch (mGunType)
	{
		case EGunType::NormalGun:
		{
			if (mCanShoot)
			{

				FActorSpawnParameters params;
				params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				params.bNoFail = true;
				params.Instigator = Cast<APawn>(GetOwner());

				check(params.Instigator != nullptr);

				AportifolioProjectile* projectile = mpWorld->SpawnActor<AportifolioProjectile>(
					this->GetActorLocation(),
					this->GetActorRotation(), params
					);

				projectile->SetDamage(50);
				projectile->SetLifeSpan(1.);

				// This makes player projectile ignore his brothers, but still collides with enemy projectiles
				projectile->GetProjectileMesh()->SetCollisionProfileName("PlayerProjectile");
	

				if (mpFireSound != nullptr)
				{
					UGameplayStatics::PlaySoundAtLocation(this, mpFireSound, GetActorLocation());
				}

				mCanShoot = false;

				// It won't be paused so we don't need to keep the reference
				FTimerHandle th;

				mpWorld->GetTimerManager().SetTimer(th, this, &AGun::EnableShooting, mShootPeriod);
			}
		
		}
		break;

		case EGunType::EnemyGun:
		{
			if (mCanShoot)
			{

				FActorSpawnParameters params;
				params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				params.bNoFail = true;
				params.Instigator = Cast<APawn>(GetOwner());

				check(params.Instigator != nullptr);

				AportifolioProjectile* projectile = mpWorld->SpawnActor<AportifolioProjectile>(
					this->GetActorLocation(),
					this->GetActorRotation(), params
					);

				projectile->SetDamage(50);
				projectile->SetLifeSpan(1.);
				projectile->GetProjectileMovement()->MaxSpeed = 1500;


				if (mpFireSound != nullptr)
				{
					UGameplayStatics::PlaySoundAtLocation(this, mpFireSound, GetActorLocation());
				}

				mCanShoot = false;

				// It won't be paused so we don't need to keep the reference
				FTimerHandle th;

				mpWorld->GetTimerManager().SetTimer(th, this, &AGun::EnableShooting, mShootPeriod);
			}
		}
		break;

		default:
		break;
	}
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();

	mpWorld = GetWorld();
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AGun::EnableShooting()
{
	mCanShoot = true;
}