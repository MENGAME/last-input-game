#include "Components/RailMovementComponent.h"

#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"

URailMovementComponent::URailMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URailMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bMovementEnabled)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner || !RailSpline)
	{
		return;
	}

	Speed = FMath::Clamp(Speed, MinSpeed, MaxSpeed);

	const float StepDeltaTime = FMath::Min(DeltaTime, MaxStepDeltaTime);
	CurrentDistanceAlongSpline += Speed * StepDeltaTime;

	const FVector Location = RailSpline->GetLocationAtDistanceAlongSpline(CurrentDistanceAlongSpline, ESplineCoordinateSpace::World);
	const FRotator Rotation = RailSpline->GetRotationAtDistanceAlongSpline(CurrentDistanceAlongSpline, ESplineCoordinateSpace::World);

	Owner->SetActorLocation(Location);
	Owner->SetActorRotation(Rotation);
}

void URailMovementComponent::SnapToSpline()
{
	AActor* Owner = GetOwner();
	if (!Owner || !RailSpline)
	{
		return;
	}

	const FVector ActorLocation = Owner->GetActorLocation();
	const FVector ClosestLocation = RailSpline->FindLocationClosestToWorldLocation(ActorLocation, ESplineCoordinateSpace::World);

	if (FVector::DistSquared(ActorLocation, ClosestLocation) > FMath::Square(SnapThreshold))
	{
		Owner->SetActorLocation(ClosestLocation);
	}
}

void URailMovementComponent::SetSpeed(float NewSpeed)
{
	Speed = FMath::Clamp(NewSpeed, MinSpeed, MaxSpeed);
}

void URailMovementComponent::SetSpline(USplineComponent* NewSpline)
{
	RailSpline = NewSpline;
}
