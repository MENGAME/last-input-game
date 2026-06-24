#include "Components/ShootingVisualsComponent.h"

#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/HitResult.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UShootingVisualsComponent::UShootingVisualsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

const UCameraComponent* UShootingVisualsComponent::GetOwnerCamera() const
{
	const AActor* Owner = GetOwner();
	return Owner ? Owner->FindComponentByClass<UCameraComponent>() : nullptr;
}

bool UShootingVisualsComponent::AimTrace(FHitResult& OutHit, FVector& OutAimPoint) const
{
	const UWorld* World = GetWorld();
	const UCameraComponent* Camera = GetOwnerCamera();
	if (!World || !Camera)
	{
		OutAimPoint = FVector::ZeroVector;
		return false;
	}

	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * AimRange;

	const FCollisionQueryParams QueryParams(FName(TEXT("AimTrace")), /*bTraceComplex=*/false, GetOwner());
	const bool bHit = World->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, QueryParams);
	OutAimPoint = bHit ? OutHit.ImpactPoint : End;
	return bHit;
}

void UShootingVisualsComponent::PlayShotVisuals()
{
	UWorld* World = GetWorld();
	const UCameraComponent* Camera = GetOwnerCamera();
	if (!World || !Camera)
	{
		return;
	}

	const FVector Start = Camera->GetComponentLocation();
	const FVector End = Start + Camera->GetForwardVector() * BeamRange;

	FHitResult Hit;
	const FCollisionQueryParams QueryParams(FName(TEXT("ShotVisualTrace")), /*bTraceComplex=*/false, GetOwner());
	const bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, QueryParams);
	const FVector BeamEnd = bHit ? Hit.ImpactPoint : End;

	// A) Muzzle flash at camera / weapon origin.
	DrawDebugSphere(World, Start, 8.0f, 8, FColor::Yellow, false, VisualLifetime, 0, 0.5f);

	// B) Trace beam from camera to the hit point (or trace end).
	DrawDebugLine(World, Start, BeamEnd, FColor::Cyan, false, VisualLifetime, 0, 1.5f);

	// C) Impact effect at the hit location.
	if (bHit)
	{
		DrawDebugSphere(World, BeamEnd, 12.0f, 8, FColor::Red, false, VisualLifetime, 0, 1.0f);
	}
}
