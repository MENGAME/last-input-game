#include "Components/RailMovementComponent.h"

#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"

URailMovementComponent::URailMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void URailMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!SplineComponent && bAutoFindSplineOnOwner)
	{
		if (AActor* Owner = GetOwner())
		{
			SplineComponent = Owner->FindComponentByClass<USplineComponent>();
		}
	}

	ClampDistanceToSpline();
	ApplySplineTransformToOwner();
}

void URailMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!SplineComponent || DeltaTime <= 0.0f)
	{
		return;
	}

	AdvanceAlongSpline(DeltaTime);
	ApplySplineTransformToOwner();
}

void URailMovementComponent::SetSplineComponent(USplineComponent* NewSplineComponent)
{
	SplineComponent = NewSplineComponent;
	ClampDistanceToSpline();
	ApplySplineTransformToOwner();
}

void URailMovementComponent::SetCurrentDistanceOnSpline(float NewDistance)
{
	CurrentDistanceOnSpline = NewDistance;
	ClampDistanceToSpline();
	ApplySplineTransformToOwner();
}

float URailMovementComponent::GetMovementSpeed(float DeltaTime) const
{
	// DeltaTime is accepted now so acceleration can be added here later.
	(void)DeltaTime;
	return Speed;
}

void URailMovementComponent::AdvanceAlongSpline(float DeltaTime)
{
	const float SplineLength = SplineComponent->GetSplineLength();
	if (SplineLength <= KINDA_SMALL_NUMBER)
	{
		CurrentDistanceOnSpline = 0.0f;
		return;
	}

	CurrentDistanceOnSpline += GetMovementSpeed(DeltaTime) * DeltaTime;

	if (bLoop)
	{
		CurrentDistanceOnSpline = FMath::Fmod(CurrentDistanceOnSpline, SplineLength);
		if (CurrentDistanceOnSpline < 0.0f)
		{
			CurrentDistanceOnSpline += SplineLength;
		}
	}
	else
	{
		ClampDistanceToSpline();
	}
}

void URailMovementComponent::ApplySplineTransformToOwner() const
{
	AActor* Owner = GetOwner();
	if (!Owner || !SplineComponent)
	{
		return;
	}

	const FVector Location = SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistanceOnSpline, ESplineCoordinateSpace::World);
	const FRotator Rotation = SplineComponent->GetRotationAtDistanceAlongSpline(CurrentDistanceOnSpline, ESplineCoordinateSpace::World);

	Owner->SetActorLocationAndRotation(Location, Rotation);
}

void URailMovementComponent::ClampDistanceToSpline()
{
	if (!SplineComponent)
	{
		CurrentDistanceOnSpline = FMath::Max(0.0f, CurrentDistanceOnSpline);
		return;
	}

	const float SplineLength = SplineComponent->GetSplineLength();
	CurrentDistanceOnSpline = FMath::Clamp(CurrentDistanceOnSpline, 0.0f, SplineLength);
}
