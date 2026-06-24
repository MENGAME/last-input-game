#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RailMovementComponent.generated.h"

class USplineComponent;

UCLASS(ClassGroup=(Movement), meta=(BlueprintSpawnableComponent))
class URailMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	URailMovementComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Snaps the owner back to the closest spline point if it deviates beyond the threshold. */
	UFUNCTION(BlueprintCallable, Category="Rail Movement")
	void SnapToSpline();

	/** Sets the movement speed, clamped between MinSpeed and MaxSpeed. */
	UFUNCTION(BlueprintCallable, Category="Rail Movement")
	void SetSpeed(float NewSpeed);

	/** Assigns the spline the owner is locked to. */
	UFUNCTION(BlueprintCallable, Category="Rail Movement")
	void SetSpline(USplineComponent* NewSpline);

protected:
	/** Spline path the owner is locked to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rail Movement")
	TObjectPtr<USplineComponent> RailSpline;

	/** Current distance travelled along the spline in Unreal units. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Rail Movement")
	float CurrentDistanceAlongSpline = 0.0f;

	/** Forward speed in Unreal units per second. Clamped between MinSpeed and MaxSpeed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rail Movement", meta=(Units="cm/s"))
	float Speed = 600.0f;

	/** Minimum allowed speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rail Movement", meta=(ClampMin="0.0", Units="cm/s"))
	float MinSpeed = 0.0f;

	/** Maximum allowed speed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rail Movement", meta=(ClampMin="0.0", Units="cm/s"))
	float MaxSpeed = 1200.0f;

	/** When false, the owner does not move along the spline. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rail Movement")
	bool bMovementEnabled = true;

	/** Maximum distance the owner may deviate from the spline before SnapToSpline corrects it. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rail Movement", meta=(ClampMin="0.0", Units="cm"))
	float SnapThreshold = 50.0f;

	/** Upper bound on the per-tick delta time, so a level-load or shader-compile hitch on the first frame cannot advance the owner past the spline in a single step. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Rail Movement", meta=(ClampMin="0.0", Units="s"))
	float MaxStepDeltaTime = 0.1f;
};
