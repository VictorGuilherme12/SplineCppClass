#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "SplineClass.generated.h"

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
};
