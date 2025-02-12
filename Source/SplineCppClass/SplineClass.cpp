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
	if (!bSnapToGround || !SplineComponent) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Snap to Ground is disabled or SplineComponent is null!"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World) 
	{
		UE_LOG(LogTemp, Warning, TEXT("World is null!"));
		return;
	}

	for (int32 i = 0; i < SplineComponent->GetNumberOfSplinePoints(); i++)
	{
		FVector PointLocation = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
		FVector Start = PointLocation + FVector(0, 0, 500);  // Aumentamos o ponto inicial
		FVector End = PointLocation + FVector(0, 0, -10000);

		FHitResult HitResult;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		UE_LOG(LogTemp, Warning, TEXT("Processing spline point %d at %s"), i, *PointLocation.ToString());

		if (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
		{
			UE_LOG(LogTemp, Warning, TEXT("Point %d hit ground at %s"), i, *HitResult.ImpactPoint.ToString());

			SplineComponent->SetLocationAtSplinePoint(i, HitResult.ImpactPoint, ESplineCoordinateSpace::World, true);
			SplineComponent->SetUpVectorAtSplinePoint(i, HitResult.ImpactNormal, ESplineCoordinateSpace::World, true);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Point %d did not hit anything!"), i);
		}
	}

	SplineComponent->UpdateSpline(); // Atualiza a spline após modificar os pontos
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