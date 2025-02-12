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

void ASplineClass::SaveSplineData()
{
	SplinePointsData.Empty();

	for (int32 i = 0; i <SplineComponent->GetNumberOfSplinePoints(); i++)
	{
		FSplinePointData PointData;
		PointData.Position = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		PointData.ArriveTangent = SplineComponent->GetArriveTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		PointData.LeaveTangent = SplineComponent->GetLeaveTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		PointData.Rotation = SplineComponent->GetRotationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		PointData.Scale = SplineComponent->GetScaleAtSplinePoint(i);
		PointData.PointType = static_cast<uint8>(SplineComponent->GetSplinePointType(i));

		SplinePointsData.Add(PointData);
	}
}

void ASplineClass::RecreateSpline()
{
	SplineComponent->ClearSplinePoints(true);

	for (const FSplinePointData& PointData : SplinePointsData)
	{
		SplineComponent->AddSplinePoint(PointData.Position, ESplineCoordinateSpace::Local, true);
		int32 NewIndex = SplineComponent->GetNumberOfSplinePoints() - 1;

		SplineComponent->SetTangentAtSplinePoint(NewIndex, PointData.ArriveTangent, ESplineCoordinateSpace::Local, true);
		SplineComponent->SetTangentAtSplinePoint(NewIndex, PointData.LeaveTangent, ESplineCoordinateSpace::Local, true);
		SplineComponent->SetRotationAtSplinePoint(NewIndex, PointData.Rotation, ESplineCoordinateSpace::Local, true);
		SplineComponent->SetScaleAtSplinePoint(NewIndex, PointData.Scale);
		SplineComponent->SetSplinePointType(NewIndex, (ESplinePointType::Type)PointData.PointType, true);
	}
}
