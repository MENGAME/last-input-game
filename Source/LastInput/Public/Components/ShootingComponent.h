#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShootingComponent.generated.h"

/**
 * Hitscan shooting component. Fires an instant line trace along the owner
 * camera's forward vector and notifies any hit actor that implements IDamageable.
 */
UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class UShootingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UShootingComponent();

	/** Fires a single hitscan shot. Respects the FireRate cooldown. Call this from input in Blueprint. */
	UFUNCTION(BlueprintCallable, Category="Shooting")
	void Fire();

protected:
	/** Damage passed to IDamageable actors on hit. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shooting", meta=(ClampMin="0.0"))
	float Damage = 20.0f;

	/** Maximum distance of the line trace in Unreal units. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shooting", meta=(ClampMin="0.0", Units="cm"))
	float TraceDistance = 10000.0f;

	/** Minimum number of seconds between shots. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shooting", meta=(ClampMin="0.0", Units="s"))
	float FireRate = 0.2f;

	/** When false, Fire() does nothing. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shooting")
	bool bCanFire = true;

private:
	/** World time (seconds) at which the next shot is allowed. */
	float NextFireTime = 0.0f;
};
