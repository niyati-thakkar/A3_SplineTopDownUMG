// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerController.h"

ACustomPlayerController::ACustomPlayerController()
{
	UserWall = CreateDefaultSubobject<AWallGenerator>(TEXT("UserWall"));
	
}


void ACustomPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent)) {
		UInputMappingContext* InputMappingContext = NewObject<UInputMappingContext>();

		// Look Action
		UInputAction* GenerateMeshAction = NewObject<UInputAction>();
		UInputAction* GenerateComponentAction = NewObject<UInputAction>();
		UInputAction* DestroyComponentAction = NewObject<UInputAction>();
		UInputAction* DestroyMeshAction = NewObject<UInputAction>();
		UInputAction* ClearLevelAction = NewObject<UInputAction>();
		GenerateMeshAction->ValueType = EInputActionValueType::Axis3D;



		InputMappingContext->MapKey(GenerateMeshAction, EKeys::LeftMouseButton);
		InputMappingContext->MapKey(GenerateComponentAction, EKeys::N);
		InputMappingContext->MapKey(DestroyComponentAction, EKeys::B);
		InputMappingContext->MapKey(DestroyMeshAction, EKeys::Z);
		InputMappingContext->MapKey(ClearLevelAction, EKeys::Tab);

		EnhancedInputComponent->BindAction(GenerateMeshAction, ETriggerEvent::Completed, this, &ACustomPlayerController::GetLocation);
		EnhancedInputComponent->BindAction(GenerateComponentAction, ETriggerEvent::Completed, this, &ACustomPlayerController::GenerateNewWallComponent);
		EnhancedInputComponent->BindAction(DestroyComponentAction, ETriggerEvent::Completed, this, &ACustomPlayerController::DestroyWallComponent);
		EnhancedInputComponent->BindAction(DestroyMeshAction, ETriggerEvent::Completed, this, &ACustomPlayerController::DestroyWallMesh);
		EnhancedInputComponent->BindAction(ClearLevelAction, ETriggerEvent::Completed, this, &ACustomPlayerController::DestroyAllWalls);

		const auto LocalPlayer = GetLocalPlayer();

		if (LocalPlayer)
		{
			if (UEnhancedInputLocalPlayerSubsystem* SubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				// Add the input mapping context to the local player subsystem
				SubSystem->AddMappingContext(InputMappingContext, 0);
			}
		}
		bShowMouseCursor = true;
		
	}
	
	
	
}
void ACustomPlayerController::GetLocation() {
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

	// Check if the hit was successful
	if (HitResult.IsValidBlockingHit())
	{
		// Print the location to the screen
		FVector Location = HitResult.Location;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Mouse Click Location: X=%f, Y=%f, Z=%f"), Location.X, Location.Y, Location.Z));

		UserWall->GenerateSplineMesh(Location);
	}
	else
	{
		DelegateLog.Execute(FString("No valid location found under the mouse cursor"), FColor::Red);
	}
	
}

void ACustomPlayerController::BeginPlay()
{
	Super::BeginPlay();
	DelegateLog.BindUFunction(this, FName("LogMessage"));
	DelegateLog.Execute(FString("Message will be shown here"), FColor::Red);
}

void ACustomPlayerController::DestroyWallMesh()
{
	if (UserWall->DestroySplineMesh()) {
		DelegateLog.Execute(FString("Wall Mesh Deleted Successfully"), FColor::Red);
	}
	else {
		DelegateLog.Execute(FString("No More Walls to Destroy"), FColor::Red);
	}
}

void ACustomPlayerController::DestroyWallComponent()
{
	if (UserWall->DestroyAllSplineMeshes()) {
		DelegateLog.Execute(FString("Wall Component Deleted Successfully"), FColor::Red);
	}
	else {
		DelegateLog.Execute(FString("No More Walls to Destroy"), FColor::Red);
	}
}

void ACustomPlayerController::DestroyAllWalls()
{
	if (UserWall->DestroyAllSplineComponents()) {
		DelegateLog.Execute(FString("Cleared Successfully"), FColor::Red);
	}
}

void ACustomPlayerController::GenerateNewWallComponent()
{
	if (UserWall->DestroySplineMesh()) {
		DelegateLog.Execute(FString("New Wall Added"), FColor::Red);
	}
}

