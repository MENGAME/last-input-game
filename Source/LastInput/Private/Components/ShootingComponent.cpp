#include "Components/ShootingComponent.h"

#include "Camera/CameraComponent.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Damageable.h"

UShootingComponent::UShootingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UShootingComponent::Fire()
{
	if (!bCanFire)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 1. Cooldown check.
	const float WorldTime = World->GetTimeSeconds();
	if (WorldTime < NextFireTime)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// 2. Camera from owner actor. 3. Forward vector.
	const UCameraComponent* Camera = Owner->FindComponentByClass<UCameraComponent>();
	if (!Camera)
	{
		return;
	}

	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * TraceDistance;

	// 4. Line trace from camera, Visibility channel, ignore owner.
	FCollisionQueryParams QueryParams(FName(TEXT("ShootingTrace")), /*bTraceComplex=*/false, Owner);

	FHitResult HitResult;
	if (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
	{
		// 5. Notify the hit actor if it implements IDamageable.
		AActor* HitActor = HitResult.GetActor();
		if (HitActor && HitActor->Implements<UDamageable>())
		{
			IDamageable::Execute_ApplyPointDamage(HitActor, Damage, HitResult);
		}
	}

	// 6. Start cooldown.
	NextFireTime = WorldTime + FireRate;
}
