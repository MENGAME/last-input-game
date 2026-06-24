#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Damageable.h"
#include "TestTarget.generated.h"

class UStaticMeshComponent;

/**
 * TEMPORARY test target. A damageable static mesh that counts and logs the hits
 * it receives from the shooting system. Used only to verify the IDamageable path
 * end-to-end; it is not a gameplay actor.
 */
UCLASS()
class ATestTarget : public AActor, public IDamageable
{
	GENERATED_BODY()

public:
	ATestTarget();

	/** IDamageable */
	virtual void ApplyPointDamage_Implementation(float Damage, const FHitResult& HitInfo) override;

	/** Number of times this target has been hit. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Test Target")
	int32 HitCount = 0;

	/** Total damage accumulated from all hits. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Test Target")
	float TotalDamage = 0.0f;

protected:
	/** Visible, collidable body so the line trace can hit it. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Test Target")
	TObjectPtr<UStaticMeshComponent> Mesh;
};
