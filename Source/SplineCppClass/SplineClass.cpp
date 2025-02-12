#include "SplineClass.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

ASplineClass::ASplineClass()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;
}

void ASplineClass::BeginPlay()
{
	Super::BeginPlay();
}

void ASplineClass::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASplineClass::SnapAllPointsToGround()
{
	if (!bSnapToGround || !SplineComponent) return;

	UWorld* World = GetWorld();
	if (!World) return;

	for (int32 i = 0; i < SplineComponent->GetNumberOfSplinePoints(); i++)
	{
		FVector PointLocation = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
		FVector Start = PointLocation + FVector(0, 0, 150);
		FVector End = PointLocation + FVector(0, 0, -10000);

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		if (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
		{
			SplineComponent->SetLocationAtSplinePoint(i, HitResult.ImpactPoint, ESplineCoordinateSpace::World, true);
			SplineComponent->SetUpVectorAtSplinePoint(i, HitResult.ImpactNormal, ESplineCoordinateSpace::World, true);
		}
	}
}
