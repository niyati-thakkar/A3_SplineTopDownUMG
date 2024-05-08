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
	NumberOfMeshes = -1;
	NumberOfSplinePoints = -1;
	
	USplineComponent* SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(SceneComponent);
	SplineComponent->AddRelativeLocation(FVector(0));
	SplineComponent->ClearSplinePoints();
	SplineComponents.Add(SplineComponent);
	NumberOfSplinePoints++;
	SplineMeshes.Add(TArray<USplineMeshComponent*>());
	NumberOfMeshes = -1;
	
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
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("%f %f"), SplineComponents.Num(), NumberOfSplinePoints));

}




bool AWallGenerator::GenerateSplineMesh(FVector SplineLocation)
{
	/*if (NumberOfMeshes == 1) {
		SplineComponent->ClearSplinePoints();
	}*/
	
	
	
	SplineLocation.Z = 0; 
	//UE_LOG(LogTemp, Warning, TEXT("%f %f"), SplineComponents.Num(), NumberOfSplinePoints);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("&d %d"), SplineComponents.Num(), NumberOfSplinePoints));
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("&d %d"), SplineMeshes.Num(), NumberOfMeshes));
	USplineComponent* SplineComponent = SplineComponents[NumberOfSplinePoints];
	SplineComponent->AddSplinePoint(SplineLocation, ESplineCoordinateSpace::Local);
	if (NumberOfMeshes++ == -1) {
		return true;
	}
	FVector StartPos, EndPos, StartTangent, EndTangent;
	SplineComponent->GetLocationAndTangentAtSplinePoint(NumberOfMeshes - 1, StartPos, StartTangent, ESplineCoordinateSpace::Local);
	SplineComponent->GetLocationAndTangentAtSplinePoint(NumberOfMeshes, EndPos, EndTangent, ESplineCoordinateSpace::Local);
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("%f %f %f"), StartPos.X, StartPos.Y, StartPos.Z));
	//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("%f %f %f"), EndPos.X, EndPos.Y, EndPos.Z));

	USplineMeshComponent* MeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());

	MeshComponent->SetStaticMesh(SourceMesh);
	MeshComponent->RegisterComponent();
	MeshComponent->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
	MeshComponent->SetupAttachment(SplineComponent);
	MeshComponent->SetForwardAxis(SplineMeshAxis);

	SplineMeshes[NumberOfSplinePoints].Add(MeshComponent);
	MeshComponent->SetRelativeScale3D(FVector(1));
	NumberOfMeshes++;
	return true;
}

bool AWallGenerator::DestroySplineMesh()
{
	if (NumberOfMeshes <= 0) {
		return false;
	}
	if (NumberOfMeshes == 1) {
		DestroyAllSplineMeshes();
		return true;
	}
	USplineComponent* SplineComponent = SplineComponents[NumberOfSplinePoints];
	if (SplineMeshes[NumberOfSplinePoints][NumberOfMeshes-1]) {
		SplineMeshes[NumberOfSplinePoints][NumberOfMeshes-1]->DestroyComponent();
		SplineMeshes[NumberOfSplinePoints][NumberOfMeshes-1] = nullptr;
		SplineMeshes[NumberOfSplinePoints].RemoveAt(NumberOfMeshes-1);
		SplineComponent->RemoveSplinePoint(NumberOfMeshes);
		NumberOfMeshes--;
	}
	
	
	return true;
}

bool AWallGenerator::DestroyAllSplineMeshes()
{
	USplineComponent* SplineComponent = SplineComponents[NumberOfSplinePoints];
	SplineComponent->ClearSplinePoints();
	for (int i = 0; i < NumberOfMeshes; i++) {
		SplineMeshes[NumberOfSplinePoints][i]->DestroyComponent();
		SplineMeshes[NumberOfSplinePoints][i] = nullptr;
	}
	SplineMeshes.RemoveAt(NumberOfSplinePoints);
	SplineComponents.Remove(SplineComponent);
	SplineComponent->DestroyComponent();
	SplineComponent = nullptr;
	NumberOfSplinePoints--;
	if (NumberOfSplinePoints == -1) {
		GenerateNewSplineComponent();
		return false;
	}
	NumberOfMeshes = SplineComponents[NumberOfSplinePoints]->GetNumberOfSplinePoints()-1;
	return true;
}

bool AWallGenerator::DestroyAllSplineComponents()
{

	int32 components = NumberOfSplinePoints;
	while (components-- >= 0) {
		DestroyAllSplineMeshes();
	}
	return true;

}

bool AWallGenerator::GenerateNewSplineComponent()
{

	USplineComponent* SplineComponent =NewObject<USplineComponent>();
	SplineComponent->AttachToComponent(SceneComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SplineComponent->AddRelativeLocation(FVector(0));
	SplineComponent->ClearSplinePoints();
	SplineComponents.Add(SplineComponent);
	SplineMeshes.Add(TArray<USplineMeshComponent*>());
	NumberOfMeshes = -1;
	NumberOfSplinePoints++;
	return true;

}
