#include "TestTarget.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/HitResult.h"

ATestTarget::ATestTarget()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void ATestTarget::ApplyPointDamage_Implementation(float Damage, const FHitResult& HitInfo)
{
	HitCount += 1;
	TotalDamage += Damage;

	UE_LOG(LogTemp, Warning, TEXT("TestTarget '%s' hit #%d for %.1f (total %.1f)"), *GetName(), HitCount, Damage, TotalDamage);
}
