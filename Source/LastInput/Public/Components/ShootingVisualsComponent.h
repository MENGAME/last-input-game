#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShootingVisualsComponent.generated.h"

class UCameraComponent;

/**
 * Visual feedback layer for the hitscan shooting system. Does NOT deal damage
 * and does NOT touch UShootingComponent — it only draws muzzle flash, the trace
 * beam, and the impact effect, and provides a short-range aim trace for crosshair
 * feedback. Call PlayShotVisuals() alongside UShootingComponent::Fire().
 */
UCLASS(ClassGroup=(Combat), meta=(BlueprintSpawnableComponent))
class UShootingVisualsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UShootingVisualsComponent();

	/** Short-range aim helper trace (NOT damage). Returns true on hit; OutAimPoint is the hit point or the trace end, for crosshair feedback. */
	UFUNCTION(BlueprintCallable, Category="Shooting Visuals")
	bool AimTrace(FHitResult& OutHit, FVector& OutAimPoint) const;

	/** Spawns shot visuals: muzzle flash at camera origin, beam from camera to hit/end, impact effect at the hit point. */
	UFUNCTION(BlueprintCallable, Category="Shooting Visuals")
	void PlayShotVisuals();

protected:
	/** Range of the aim helper trace used for crosshair feedback. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shooting Visuals", meta=(ClampMin="0.0", Units="cm"))
	float AimRange = 5000.0f;

	/** Range of the visual beam; keep in sync with the shooting TraceDistance. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shooting Visuals", meta=(ClampMin="0.0", Units="cm"))
	float BeamRange = 10000.0f;

	/** How long the beam / impact / muzzle visuals persist. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Shooting Visuals", meta=(ClampMin="0.0", Units="s"))
	float VisualLifetime = 0.05f;

private:
	/** Resolves the camera used as the trace/visual origin. */
	const UCameraComponent* GetOwnerCamera() const;
};
