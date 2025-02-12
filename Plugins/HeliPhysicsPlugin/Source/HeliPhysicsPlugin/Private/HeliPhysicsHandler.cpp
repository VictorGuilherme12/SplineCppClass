#include "HeliPhysicsHandler.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Engine/StaticMeshActor.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"


AHeliPhysicsHandler::AHeliPhysicsHandler()
{
	PrimaryActorTick.bCanEverTick = true;

		
	// Criando o Child Actor Component
	Root = CreateDefaultSubobject<UChildActorComponent>(TEXT("ChildBody"));
	Root->SetupAttachment(RootComponent);
}

void AHeliPhysicsHandler::BeginPlay()
{
	Super::BeginPlay();

	OnSelectUpdated();

	// 🔄 **Salva a rotação inicial das hélices SOMENTE se não estiverem definidas**
	if (MainRotor && InitialMainRotorRotation.IsZero())
	{
		InitialMainRotorRotation = MainRotor->GetActorRotation();
	}

	if (TailRotor && InitialTailRotorRotation.IsZero())
	{
	
		InitialTailRotorRotation = TailRotor->GetActorRotation();
	}

	// 🔄 **Garante que os rotores sempre fiquem anexados ao helicóptero**
	if (MainRotor)
	{
		MainRotor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}
    
	if (TailRotor)
	{
		TailRotor->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	}

	// 🔹 Garante que o helicóptero comece na posição correta da spline
	if (SplinePath)
	{
		if (USplineComponent* SplineComponent = SplinePath->FindComponentByClass<USplineComponent>())
		{
			DistanceTraveled = 0.0f;  // Reinicia o trajeto
			const FVector StartLocation = SplineComponent->GetLocationAtDistanceAlongSpline(DistanceTraveled, ESplineCoordinateSpace::World);
			const FRotator StartRotation = SplineComponent->GetRotationAtDistanceAlongSpline(DistanceTraveled, ESplineCoordinateSpace::World);

			SetActorLocation(StartLocation);
			SetActorRotation(StartRotation);
		}
	}
}

void AHeliPhysicsHandler::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 🚁 Atualiza a posição ao longo da spline
	MoveAlongSpline();

	// 🔄 Atualiza a rotação das hélices independentemente
	UpdateRotorRotation();
}


void AHeliPhysicsHandler::OnSelectUpdated()
{
    if (Select == nullptr) // 🛑 Se Select for NULL, remove tudo
    {
        // 🔄 Obtém todos os atores anexados ao RootComponent e os desanexa
        TArray<AActor*> AttachedActors;
        GetAttachedActors(AttachedActors);

        for (AActor* ChildActor : AttachedActors)
        {
            if (ChildActor)
            {
                ChildActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
            }
        }

        return; // Sai da função, pois não há mais nada para processar
    }

    if (IsValid(Select) && IsValid(RootComponent))
    {
        // Se for um StaticMeshActor, verificar mobilidade
        if (AStaticMeshActor* MeshActor = Cast<AStaticMeshActor>(Select))
        {
            if (UStaticMeshComponent* MeshComponent = MeshActor->GetStaticMeshComponent())
            {
                if (MeshComponent->Mobility == EComponentMobility::Static)
                {
                    MeshComponent->SetMobility(EComponentMobility::Movable);
                }
            }
        }

        // Obtém a rotação original do Body ANTES de mover
        const FRotator OriginalRotation = Select->GetActorRotation();

        // Move o Select para a posição do BP_Heli, mas mantendo sua rotação original
        Select->SetActorLocation(GetActorLocation());

        // *Anexa ao RootComponent preservando rotação*
        Select->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

        // *Força a rotação original após o anexo*
        Select->SetActorRotation(OriginalRotation);

        // *Força hierarquia no World Outliner*
        Select->SetOwner(this);
    }
}
void AHeliPhysicsHandler::MoveAlongSpline()
{
	if (!SplinePath)
	{
		return;
	}

	USplineComponent* SplineComponent = SplinePath->FindComponentByClass<USplineComponent>();
	if (!SplineComponent)
	{
		return;
	}

	float SplineLength = SplineComponent->GetSplineLength();
    
	// 🛠️ Garante que DistanceTraveled não ultrapasse os limites da spline
	DistanceTraveled = FMath::Clamp(DistanceTraveled, 0.0f, SplineLength);

	// 🚁 Obtém a nova posição da spline
	FVector NewLocation = SplineComponent->GetLocationAtDistanceAlongSpline(DistanceTraveled, ESplineCoordinateSpace::World);
    
	// 🔄 Obtém a rotação da spline e adiciona a rotação manual do usuário
	FRotator SplineRotation = SplineComponent->GetRotationAtDistanceAlongSpline(DistanceTraveled, ESplineCoordinateSpace::World);
	FRotator CustomRotation = FRotator(InitialPitch, InitialYaw, InitialRoll);
	FRotator FinalRotation = SplineRotation + CustomRotation;

	// 🚁 Atualiza a posição e rotação do helicóptero
	SetActorLocation(NewLocation);
	SetActorRotation(FinalRotation);

}
void AHeliPhysicsHandler::InitializeSplinePosition()
{
	if (!SplinePath)
	{
		return;
	}

	USplineComponent* PathSpline = SplinePath->FindComponentByClass<USplineComponent>();
	if (!PathSpline)
	{
		return;
	}

	// Define a posição inicial na spline
	DistanceTraveled = 0.0f;
	FVector StartLocation = PathSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);

	// Cria um Rotator com os valores definidos pelo usuário
	FRotator StartRotation(InitialPitch, InitialYaw, InitialRoll);

	// Aplica posição e rotação ao helicóptero
	SetActorLocation(StartLocation);
	SetActorRotation(StartRotation);
}

void AHeliPhysicsHandler::UpdateRotorRotation()
{

	if (!MainRotor || !TailRotor)
	{
		return;
	}

	float DeltaTime = GetWorld()->GetDeltaSeconds();

	// 🔄 **Interpolação suave do RPM**
	CurrentMainRotorRpm = FMath::FInterpTo(CurrentMainRotorRpm, MainRotorRpm, DeltaTime, 5.0f);
	CurrentTailRotorRpm = FMath::FInterpTo(CurrentTailRotorRpm, TailRotorRpm, DeltaTime, 5.0f);

	// 🌀 **Converter RPM para rotação por segundo**
	float MainRotorDegreesPerSecond = (CurrentMainRotorRpm / 60.0f) * 360.0f;
	float TailRotorDegreesPerSecond = (CurrentTailRotorRpm / 60.0f) * 360.0f;

	// 🚁 **Aplicar rotação ao MainRotor**
	if (MainRotor)
	{
		if (MainRotorRpm > 0.0f)
		{
			MainRotor->AddActorLocalRotation(FRotator(0.0f, MainRotorDegreesPerSecond * DeltaTime, 0.0f));
		}
	}

	// 🚁 **Aplicar rotação ao TailRotor**
	if (TailRotor)
	{
		if (TailRotorRpm > 0.0f)
		{
			TailRotor->AddActorLocalRotation(FRotator(TailRotorDegreesPerSecond * DeltaTime, 0.0f, 0.0f));
		}
	}

}


#if WITH_EDITOR
void AHeliPhysicsHandler::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHeliPhysicsHandler, DistanceTraveled) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AHeliPhysicsHandler, InitialPitch) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AHeliPhysicsHandler, InitialYaw) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AHeliPhysicsHandler, InitialRoll))
	{
		MoveAlongSpline();

		// 🔄 **Força atualização no Editor para evitar "travamento"**
		MarkPackageDirty();
		GetWorld()->Tick(ELevelTick::LEVELTICK_ViewportsOnly, 0.0f);
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHeliPhysicsHandler, MainRotorRpm) ||
		PropertyName == GET_MEMBER_NAME_CHECKED(AHeliPhysicsHandler, TailRotorRpm))
	{
		UpdateRotorRotation();
	}

	if (PropertyName == GET_MEMBER_NAME_CHECKED(AHeliPhysicsHandler, Select))
	{
		OnSelectUpdated();
	}
}
#endif