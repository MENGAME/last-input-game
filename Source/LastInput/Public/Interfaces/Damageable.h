#pragma once

#include "CoreMinimal.h"
#include "Engine/HitResult.h"
#include "UObject/Interface.h"
#include "Damageable.generated.h"

UINTERFACE(Blueprintable)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implemented by anything that can receive damage from a hit.
 *
 * The implementation is left to gameplay actors (C++ or Blueprint); this
 * interface only defines the contract the shooting system calls into.
 */
class IDamageable
{
	GENERATED_BODY()

public:
	/** Called when this object is hit. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Damage")
	void ApplyPointDamage(float Damage, const FHitResult& HitInfo);
};
