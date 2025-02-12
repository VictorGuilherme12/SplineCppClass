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

	UPROPERTY(EditAnywhere, Category = "Spline|Spline Settings")
	bool bSnapToGround = false;

	UFUNCTION()
	void SnapAllPointsToGround();

	UFUNCTION()
	void SaveSplineData();

	UFUNCTION()
	void RecreateSpline();

	UFUNCTION()
	void MoveActorToFirstPointAndRelocate();

	UFUNCTION()
	void AntiTwist();

private:
	TArray<FSplinePointData> SplinePointsData;
	TArray<FVector> SplinePointLocations;


#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};



