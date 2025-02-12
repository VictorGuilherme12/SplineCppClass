#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/ChildActorComponent.h"
#include "HeliPhysicsHandler.generated.h"

UCLASS()
class HELIPHYSICSPLUGIN_API AHeliPhysicsHandler : public AActor
{
	GENERATED_BODY()
	
public:
	AHeliPhysicsHandler();
	virtual void Tick(float DeltaTime) override;

	virtual bool ShouldTickIfViewportsOnly() const override
	{
		return true;
	}

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Helicopter", meta = (DisplayAfter = "Transform"))
	UChildActorComponent* Root;

	// Body Select
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Helicopter", meta = (ExposeOnSpawn = "true"))
	AActor* Select;

	// Spline Actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Helicopter")
	AActor* SplinePath;

#pragma region Settings	
	// Distance traveled at spline - AGORA PODE SER ANIMADO NO SEQUENCER
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp, Category = "Helicopter|Settings", meta = (DisplayAfter = ""))
	float DistanceTraveled = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Helicopter|Settings")
	float TravelSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp, Category = "Helicopter|Settings", meta = (ClampMin = "-360.0", ClampMax = "360.0"))
	float InitialPitch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp, Category = "Helicopter|Settings", meta = (ClampMin = "-180.0", ClampMax = "180.0"))
	float InitialYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp, Category = "Helicopter|Settings", meta = (ClampMin = "-180.0", ClampMax = "180.0"))
	float InitialRoll;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Helicopter")
	AActor* MainRotor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Helicopter")
	AActor* TailRotor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Helicopter|Settings")
	bool bMainRotorUseLocalRotation = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Helicopter|Settings")
	bool bTailRotorUseLocalRotation = false;

	// RPM dos rotores - ANIMÁVEIS NO SEQUENCER
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp, Category = "Helicopter|Settings", meta = (ClampMin = "0.0", ClampMax = "600.0"))
	float MainRotorRpm = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Interp, Category = "Helicopter|Settings", meta = (ClampMin = "0.0", ClampMax = "6000.0"))
	float TailRotorRpm = 0.0f;

#pragma endregion

private:
	FRotator InitialMainRotorRotation;
	FRotator InitialTailRotorRotation;
	float CurrentMainRotorRpm = 0.0f;
	float CurrentTailRotorRpm = 0.0f;
	float AccelerationSpeed = 5.0f;

#pragma region Functions
	UFUNCTION()
	void OnSelectUpdated();

	UFUNCTION()
	void MoveAlongSpline();

	UFUNCTION()
	void InitializeSplinePosition();
	
	void UpdateRotorRotation();

#pragma endregion

#if WITH_EDITOR
	/** Função chamada quando uma propriedade é modificada no Editor ou Sequencer */
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
