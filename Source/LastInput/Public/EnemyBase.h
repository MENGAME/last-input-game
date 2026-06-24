#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Damageable.h"
#include "EnemyBase.generated.h"

class UStaticMeshComponent;

/**
 * Minimal damageable enemy. No AI, no movement, no behavior — just HP and the
 * IDamageable contract: take damage, die at zero.
 */
UCLASS()
class AEnemyBase : public AActor, public IDamageable
{
	GENERATED_BODY()

public:
	AEnemyBase();

	/** IDamageable */
	virtual void ApplyPointDamage_Implementation(float Damage, const FHitResult& HitInfo) override;

	/** Current hit points. The enemy is destroyed when this reaches zero. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Enemy", meta=(ClampMin="0.0"))
	float HP = 100.0f;

protected:
	/** Visible, collidable body so the shooting line trace can hit it. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Enemy")
	TObjectPtr<UStaticMeshComponent> Mesh;
};
