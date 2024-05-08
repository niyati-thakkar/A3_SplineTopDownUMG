// Fill out your copyright notice in the Description page of Project Settings.


#include "WallGenerator.h"

// Sets default values
AWallGenerator::AWallGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("USceneComponent"));
	RootComponent = SceneComponent;
	SourceMesh = CreateDefaultSubobject<UStaticMesh>(TEXT("SourceMesh"));
	// Set the static mesh

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StaticMeshAsset(TEXT("/Game/StarterContent/Architecture/Wall_400x400"));
	if (StaticMeshAsset.Succeeded())
	{
		SourceMesh = StaticMeshAsset.Object;

		// Set the material
		static ConstructorHelpers::FObjectFinder<UMaterialInstance> MaterialAsset(TEXT("/Game/StarterContent/Materials/M_Basic_Wall.M_Basic_Wall"));
		if (MaterialAsset.Succeeded())
		{
			WallMaterial = MaterialAsset.Object;
			if (WallMaterial)
			{
				SourceMesh->SetMaterial(0, WallMaterial);
			}
		}
	}

	USplineComponent* SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(SceneComponent);
	SplineComponent->AddRelativeLocation(FVector(0));
	SplineComponent->ClearSplinePoints();
	SplineComponents.Add(SplineComponent);

}

// Called when the game starts or when spawned
void AWallGenerator::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AWallGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("%f %f"), SplineComponents.Num(), NumberOfSplineComponents));

}




bool AWallGenerator::GenerateSplineMesh(FVector SplineLocation)
{
	/*if (NumberOfMeshes == 1) {
		SplineComponent->ClearSplinePoints();
	}*/



	SplineLocation.Z = 0;
	//UE_LOG(LogTemp, Warning, TEXT("%f %f"), SplineComponents.Num(), NumberOfSplineComponents);
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("&d %d"), SplineComponents.Num(), NumberOfSplineComponents));
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("&d %d"), SplineMeshes.Num(), NumberOfMeshes));
	int32 SplineComponentIndex = SplineComponents.Num() - 1;
	USplineComponent* SplineComponent = SplineComponents[SplineComponentIndex];
	SplineComponent->AddSplinePoint(SplineLocation, ESplineCoordinateSpace::Local);
	int32 SplinePoints = SplineComponent->GetNumberOfSplinePoints();
	if (SplinePoints == 1) {
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("add one more point") ));
		return true;
	}

	FVector StartPos, EndPos, StartTangent, EndTangent;
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("%f"), index));
	SplineComponent->GetLocationAndTangentAtSplinePoint(SplinePoints - 2, StartPos, StartTangent, ESplineCoordinateSpace::Local);
	SplineComponent->GetLocationAndTangentAtSplinePoint(SplinePoints - 1, EndPos, EndTangent, ESplineCoordinateSpace::Local);
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("%f %f %f"), StartPos.X, StartPos.Y, StartPos.Z));
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("%f %f %f"), EndPos.X, EndPos.Y, EndPos.Z));

	USplineMeshComponent* MeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());

	MeshComponent->SetStaticMesh(SourceMesh);
	MeshComponent->RegisterComponent();
	MeshComponent->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
	MeshComponent->SetupAttachment(SplineComponent);
	MeshComponent->SetForwardAxis(SplineMeshAxis);

	SplineMeshes.Add(MeshComponent);
	MeshComponent->SetRelativeScale3D(FVector(1));
	return true;
}

bool AWallGenerator::DestroySplineMesh()
{
	
	if (SplineMeshes.Num() == -1) {
		return false;
	}

	int32 SplineComponentIndex = SplineComponents.Num() - 1;
	USplineComponent* SplineComponent = SplineComponents[SplineComponentIndex];
	int32 NumberOfMeshes = SplineComponent->GetNumberOfSplinePoints() - 1;
	
	if (NumberOfMeshes >= 1) {
		int32 MeshIndex = SplineMeshes.Num() - 1;
		SplineMeshes[MeshIndex]->DestroyComponent();
		SplineMeshes[MeshIndex] = nullptr;
		SplineMeshes.RemoveAt(MeshIndex);
		SplineComponent->RemoveSplinePoint(NumberOfMeshes);
		
	}
	if (NumberOfMeshes <= 1) {
		SplineComponent->ClearSplinePoints();
		SplineComponents[SplineComponentIndex]->DestroyComponent();
		SplineComponents[SplineComponentIndex] = nullptr;
		SplineComponents.RemoveAt(SplineComponentIndex);
		if (SplineComponentIndex == 0) {
			GenerateNewSplineComponent();
		}
		
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("%d"), NumberOfMeshes));
	return true;
}

bool AWallGenerator::DestroyAllSplineMeshes()
{
	int32 SplineComponentIndex = SplineComponents.Num() - 1;
	USplineComponent* SplineComponent = SplineComponents[SplineComponentIndex];
	int32 SplinePoints = SplineComponent->GetNumberOfSplinePoints();
	if (SplinePoints == 0) {
		return DestroySplineMesh();
	}
	while (SplinePoints-- > 1) {
		DestroySplineMesh();
	}
	return true;
}

bool AWallGenerator::DestroyAllSplineComponents()
{

	int32 SplineComponentIndex = SplineComponents.Num() - 1;
	if (SplineComponentIndex == -1) return false;
	while (SplineComponentIndex-- >= 0) {
		DestroyAllSplineMeshes();
	}
	return true;

}

bool AWallGenerator::GenerateNewSplineComponent()
{
	int32 SplineComponentIndex = SplineComponents.Num() - 1;
	
	if (SplineComponentIndex >= 0){
		USplineComponent* SplineComponent = SplineComponents[SplineComponentIndex];
		if (SplineComponent->GetNumberOfSplinePoints() <= 1) {
			SplineComponent->ClearSplinePoints();
			return true;
		}
	}

	USplineComponent* SplineComponent = NewObject<USplineComponent>();
	SplineComponent->AttachToComponent(SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SplineComponent->AddRelativeLocation(FVector(0));
	SplineComponent->ClearSplinePoints();
	SplineComponents.Add(SplineComponent);
	return true;

}
