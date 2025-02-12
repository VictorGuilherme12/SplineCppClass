#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "SplineClass.generated.h"

USTRUCT(BlueprintType)
struct FSplinePointData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Position;

	UPROPERTY()
	FVector ArriveTangent;

	UPROPERTY()
	FVector LeaveTangent;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector Scale;

	UPROPERTY()
	uint8 PointType;
};

UCLASS()
class SPLINECPPCLASS_API ASplineClass : public AActor
{
	GENERATED_BODY()

public:
	ASplineClass();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, Category = "Spline")
	USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere, Category = "Spline Settings")
	bool bSnapToGround = true;

	UFUNCTION(CallInEditor, Category = "Spline")
	void SnapAllPointsToGround();

	UFUNCTION(CallInEditor, Category = "Spline")
	void SaveSplineData();

	UFUNCTION(CallInEditor, Category = "Spline")
	void RecreateSpline();

private:
	TArray<FSplinePointData> SplinePointsData;
};
