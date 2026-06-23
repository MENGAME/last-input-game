#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RailMovementComponent.generated.h"

class USplineComponent;

/**
 * Moves the owning actor along a spline at a configurable speed.
 *
 * The tick flow is intentionally split into small methods so acceleration,
 * braking, and scripted pacing can be added later without replacing the
 * component's public API.
 */
UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class URailMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URailMovementComponent();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Assigns the spline this component should follow. */
	UFUNCTION(BlueprintCallable, Category="Rail Movement")
	void SetSplineComponent(USplineComponent* NewSplineComponent);

	/** Moves to a specific distance along the current spline. */
	UFUNCTION(BlueprintCallable, Category="Rail Movement")
	void SetCurrentDistanceOnSpline(float NewDistance);

	/** Returns the active spline used for movement. */
	UFUNCTION(BlueprintPure, Category="Rail Movement")
	USplineComponent* GetSplineComponent() const { return SplineComponent; }

	/** Returns the current distance along the active spline. */
	UFUNCTION(BlueprintPure, Category="Rail Movement")
	float GetCurrentDistanceOnSpline() const { return CurrentDistanceOnSpline; }

protected:
	/** Spline followed by the owning actor. If empty, the component can auto-find one on the owner. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rail Movement")
	TObjectPtr<USplineComponent> SplineComponent;

	/** Movement speed in Unreal units per second. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rail Movement", meta=(ClampMin="0.0", Units="cm/s"))
	float Speed = 600.0f;

	/** Current distance along the spline in Unreal units. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rail Movement")
	float CurrentDistanceOnSpline = 0.0f;

	/** If true, the owner wraps back to the beginning when it reaches the spline end. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rail Movement")
	bool bLoop = false;

	/** If true, BeginPlay will use the owner's first USplineComponent when no spline is assigned. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rail Movement")
	bool bAutoFindSplineOnOwner = true;

	/** Hook point for a future acceleration system. */
	virtual float GetMovementSpeed(float DeltaTime) const;

	/** Advances CurrentDistanceOnSpline while respecting spline length and looping. */
	void AdvanceAlongSpline(float DeltaTime);

	/** Applies the spline location and rotation at CurrentDistanceOnSpline to the owner. */
	void ApplySplineTransformToOwner() const;

	/** Keeps CurrentDistanceOnSpline valid for the active spline. */
	void ClampDistanceToSpline();
};
