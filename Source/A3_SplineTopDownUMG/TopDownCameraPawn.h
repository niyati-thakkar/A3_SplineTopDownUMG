// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "TopDownCameraPawn.generated.h"



UCLASS()
class A3_SPLINETOPDOWNUMG_API ATopDownCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATopDownCameraPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleDefaultsOnly, Category=Defaults)
	UCapsuleComponent* CapsuleComponent;

	UPROPERTY(VisibleDefaultsOnly, Category=Defaults)
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleDefaultsOnly, Category=Defaults)
	UCameraComponent* Camera;

	UPROPERTY(VisibleDefaultsOnly, Category=Defaults)
	UFloatingPawnMovement* FloatingPawnMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera|Zoom")
	float ZoomStep = 10.f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
    float PerspectiveCameraPitch = -30.0f;

    UPROPERTY(EditAnywhere, Category = "Camera Settings")
    float PerspectiveTargetOffsetZ = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Settings")
    float DefaultTargetOffsetZ = 10.0f;

	void CameraZoom(const FInputActionValue& ZoomAction);
	
	void Look(const FInputActionValue& LookAction);

	void Move(const FInputActionValue& LookAction);

	
	//UPROPERTY(VisibleDefaultsOnly, Category=Defaults)

	

};
