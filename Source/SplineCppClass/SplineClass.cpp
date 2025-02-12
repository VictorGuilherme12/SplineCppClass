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
		SplineComponent->SetSplinePointType(NewIndex, static_cast<ESplinePointType::Type>(PointData.PointType), true);
	}
}

void ASplineClass::MoveActorToFirstPointAndRelocate()
{
	if (!SplineComponent || SplinePointsData.Num() == 0) return;

	const FVector FirstPointLocation = SplinePointsData[0].Position;
	SetActorLocation(FirstPointLocation);

	int32 NumPoints = SplineComponent->GetNumberOfSplinePoints();

	for (int32 i = 0; i < NumPoints; i++)
	{
		if (i < SplinePointsData.Num())
		{
			SplineComponent->SetLocationAtSplinePoint(i, SplinePointsData[i].Position, ESplineCoordinateSpace::World, true);
		}
	}

	SplineComponent->UpdateSpline();
	UE_LOG(LogTemp, Warning, TEXT("Moved actor and relocated %d spline points."), NumPoints);
}

void ASplineClass::AntiTwist()
{
	if (!SplineComponent) return;

	const int32 NumPoints = SplineComponent->GetNumberOfSplinePoints();

	for (int32 i = 0; i < NumPoints; i++)
	{
		FVector UpVector = SplineComponent->GetUpVectorAtSplinePoint(i, ESplineCoordinateSpace::Local);

		UpVector = FVector(UpVector.X, UpVector.Y, 1.0f);

		SplineComponent->SetUpVectorAtSplinePoint(i, UpVector.GetSafeNormal(), ESplineCoordinateSpace::Local, true);
	}

	SplineComponent->UpdateSpline();

	UE_LOG(LogTemp, Warning, TEXT("AntiTwist() applied - Up Vectors aligned"));
}



#if WITH_EDITOR
void ASplineClass::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.Property)
	{
		FName PropertyName = PropertyChangedEvent.Property->GetFName();
		UE_LOG(LogTemp, Warning, TEXT("Property %s changed! Updating spline..."), *PropertyName.ToString());

		// Atualiza a spline quando qualquer propriedade for alterada
		SplineComponent->UpdateSpline();
		Modify(); // Marca a spline como modificada no editor
		MarkPackageDirty(); // Garante que a mudança seja salva

		// Se foi uma mudança na propriedade "bSnapToGround", ativa o snap automático
		if (PropertyName == GET_MEMBER_NAME_CHECKED(ASplineClass, bSnapToGround))
		{
			if (bSnapToGround)
			{
				SnapAllPointsToGround();
			}
		}

		// Se qualquer ponto da spline for alterado, salvamos os dados da spline novamente
		if (PropertyName == GET_MEMBER_NAME_CHECKED(USplineComponent, SplineCurves))
		{
			UE_LOG(LogTemp, Warning, TEXT("Spline points changed, saving new spline data..."));
			SaveSplineData();
		}
	}
}
#endif