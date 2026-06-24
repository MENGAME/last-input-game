#include "EnemyBase.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/HitResult.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

void AEnemyBase::ApplyPointDamage_Implementation(float Damage, const FHitResult& HitInfo)
{
	HP -= Damage;

	UE_LOG(LogTemp, Log, TEXT("Enemy '%s' took %.1f damage, HP=%.1f"), *GetName(), Damage, HP);

	if (HP <= 0.0f)
	{
		Destroy();
	}
}
