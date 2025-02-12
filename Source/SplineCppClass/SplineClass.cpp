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

	int32 NumPoints = SplineComponent->GetNumberOfSplinePoints();
	for (int32 i = 0; i < NumPoints; i++)
	{
		FSplinePointData PointData;
		PointData.Position = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
		PointData.ArriveTangent = SplineComponent->GetArriveTangentAtSplinePoint(i, ESplineCoordinateSpace::World);
		PointData.LeaveTangent = SplineComponent->GetLeaveTangentAtSplinePoint(i, ESplineCoordinateSpace::World);
		PointData.Rotation = SplineComponent->GetRotationAtSplinePoint(i, ESplineCoordinateSpace::World);
		PointData.Scale = SplineComponent->GetScaleAtSplinePoint(i);
		PointData.PointType = static_cast<uint8>(SplineComponent->GetSplinePointType(i));

		SplinePointsData.Add(PointData);
	}

	UE_LOG(LogTemp, Warning, TEXT("Spline data saved with %d points"), NumPoints);
}


void ASplineClass::RecreateSpline()
{
	if (SplinePointsData.Num() == 0) return;

	SplineComponent->ClearSplinePoints(true);

	for (int32 i = 0; i < SplinePointsData.Num(); i++)
	{
		SplineComponent->AddSplinePoint(SplinePointsData[i].Position, ESplineCoordinateSpace::World, true);
        
		SplineComponent->SetTangentAtSplinePoint(i, SplinePointsData[i].ArriveTangent, ESplineCoordinateSpace::World, true);
		SplineComponent->SetTangentAtSplinePoint(i, SplinePointsData[i].LeaveTangent, ESplineCoordinateSpace::World, true);
		SplineComponent->SetRotationAtSplinePoint(i, SplinePointsData[i].Rotation, ESplineCoordinateSpace::World, true);
		SplineComponent->SetScaleAtSplinePoint(i, SplinePointsData[i].Scale);
		SplineComponent->SetSplinePointType(i, static_cast<ESplinePointType::Type>(SplinePointsData[i].PointType), true);
	}

	UE_LOG(LogTemp, Warning, TEXT("Spline recreated correctly with %d points"), SplineComponent->GetNumberOfSplinePoints());

	SplineComponent->UpdateSpline();
}


void ASplineClass::MoveActorToFirstPointAndRelocate()
{
	if (!SplineComponent || SplinePointsData.Num() == 0) return;

	// Pega o primeiro ponto da spline real
	const FVector FirstPointLocation = SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
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
		
		// Se qualquer ponto da spline for alterado, salvamos os dados da spline novamente
		if (PropertyName == GET_MEMBER_NAME_CHECKED(USplineComponent, SplineCurves))
		{
			UE_LOG(LogTemp, Warning, TEXT("Spline points changed, saving new spline data..."));
			SaveSplineData();
		}
	}
}
#endif