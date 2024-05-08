// Fill out your copyright notice in the Description page of Project Settings.


#include "TopDownCameraPawn.h"
#include "VectorTypes.h"

// Sets default values
ATopDownCameraPawn::ATopDownCameraPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->SetCapsuleHalfHeight(90);
	CapsuleComponent->SetCapsuleRadius(30);
	RootComponent = CapsuleComponent;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->TargetArmLength = 20;
	SpringArmComponent->SetupAttachment(RootComponent);

	SpringArmComponent->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	FloatingPawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("FloatingPawnMovement"));
}

// Called when the game starts or when spawned
void ATopDownCameraPawn::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void ATopDownCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Called to bind functionality to input
void ATopDownCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		UInputMappingContext* InputMappingContext = NewObject<UInputMappingContext>();

		// Look Action
		UInputAction* LookAction = NewObject<UInputAction>();
		LookAction->ValueType = EInputActionValueType::Axis2D;

		UInputModifierNegate* LookActionMouseNegateModifier = NewObject<UInputModifierNegate>();
		LookActionMouseNegateModifier->bX = false;
		LookActionMouseNegateModifier->bY = true;
		LookActionMouseNegateModifier->bZ = false;

		FEnhancedActionKeyMapping& LookActionMappedContext = InputMappingContext->MapKey(LookAction, EKeys::Mouse2D);
		LookActionMappedContext.Modifiers.Add(LookActionMouseNegateModifier);


		// Move Action 
		UInputAction* MoveAction = NewObject<UInputAction>();
		MoveAction->ValueType = EInputActionValueType::Axis3D;

		UInputModifierNegate* MoveActionNegateModifier = NewObject<UInputModifierNegate>();
		MoveActionNegateModifier->bX = true;
		MoveActionNegateModifier->bY = true;
		MoveActionNegateModifier->bZ = true;

		UInputModifierSwizzleAxis* SwizzleModifierDKey = NewObject<UInputModifierSwizzleAxis>();
		SwizzleModifierDKey->Order = EInputAxisSwizzle::YXZ;

		// Zoom Action 
		UInputAction* ZoomAction = NewObject<UInputAction>();
		ZoomAction->ValueType = EInputActionValueType::Axis3D;

		UInputModifierNegate* ZoomActionNegateModifier = NewObject<UInputModifierNegate>();
		MoveActionNegateModifier->bX = true;
		MoveActionNegateModifier->bY = true;
		MoveActionNegateModifier->bZ = true;

		// Mapping Keys
		InputMappingContext->MapKey(MoveAction, EKeys::W);

		FEnhancedActionKeyMapping& MoveForwardInputMapping = InputMappingContext->MapKey(MoveAction, EKeys::S);
		MoveForwardInputMapping.Modifiers.Add(MoveActionNegateModifier);

		FEnhancedActionKeyMapping& MoveRightInputMapping = InputMappingContext->MapKey(MoveAction, EKeys::D);
		MoveRightInputMapping.Modifiers.Add(SwizzleModifierDKey);

		FEnhancedActionKeyMapping& MoveLeftInputMapping = InputMappingContext->MapKey(MoveAction, EKeys::A);
		MoveLeftInputMapping.Modifiers.Add(SwizzleModifierDKey);
		MoveLeftInputMapping.Modifiers.Add(MoveActionNegateModifier);

		InputMappingContext->MapKey(MoveAction, EKeys::MouseScrollDown);

		FEnhancedActionKeyMapping& MouseUpInputMapping = InputMappingContext->MapKey(MoveAction, EKeys::MouseScrollUp);
		MouseUpInputMapping.Modifiers.Add(ZoomActionNegateModifier);

		// Bindings
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATopDownCameraPawn::Look);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATopDownCameraPawn::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATopDownCameraPawn::CameraZoom);


		if (const APlayerController* PlayerController = Cast<APlayerController>(Controller)) {
			if (UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
				//SubSystem->ClearAllMappings();
				SubSystem->AddMappingContext(InputMappingContext, 0);
			}
		}

	}
}

void ATopDownCameraPawn::CameraZoom(const FInputActionValue& ActionValue)
{
	float Value = ActionValue.Get<float>();

	float TargetArmLength = SpringArmComponent->TargetArmLength;
	float TargetOffsetZ = SpringArmComponent->TargetOffset.Z;

	if (TargetArmLength >= -460) {
		SpringArmComponent->TargetOffset.Z -= Value * 20;
	}
	if (TargetOffsetZ <= 1000) {
		SpringArmComponent->TargetArmLength -= Value * 9.2;
	}

	SpringArmComponent->TargetArmLength = FMath::Clamp(SpringArmComponent->TargetArmLength, -500, -0);
	SpringArmComponent->TargetOffset.Z = FMath::Clamp(SpringArmComponent->TargetOffset.Z, 0, 3000);

	float Angle;
	if (SpringArmComponent->TargetArmLength == 0) {
		Angle = -90;
	}
	else {
		Angle = FMath::RadiansToDegrees(atan(SpringArmComponent->TargetOffset.Z / SpringArmComponent->TargetArmLength));
		Angle = FMath::Clamp(Angle, -90, 0);
	}

	if (Angle < -90) {
		Angle = -90;
	}
	if (Angle > 0) {
		Angle = 0;
	}

	FString Offset_Z = "Offset_Z: " + FString::SanitizeFloat(SpringArmComponent->TargetOffset.Z);
	FString ArmLength = "ArmLength: " + FString::SanitizeFloat(SpringArmComponent->TargetArmLength);
	FString Angle_ = "Angle: " + FString::SanitizeFloat(Angle);

	Camera->SetRelativeRotation(FRotator(Angle, 0, 0));

	/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("ROTATION: %f"), *Camera->GetRelativeRotation().ToString()));*/

}

void ATopDownCameraPawn::Look(const FInputActionValue& LookAction)
{
	FVector2D LookActionVector = LookAction.Get<FVector2D>();
	AddControllerYawInput(LookActionVector.X);
	AddControllerPitchInput(LookActionVector.Y);
}

void ATopDownCameraPawn::Move(const FInputActionValue& MoveAction)
{
	FVector MovementVector = MoveAction.Get<FVector>();
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Cyan, "This ");
	FRotator Rotation = Controller->GetControlRotation();

	FVector ForwardDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::X);
	FVector RightDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Y);
	FVector UpDirection = FRotationMatrix(Rotation).GetUnitAxis(EAxis::Z);

	AddMovementInput(ForwardDirection, MovementVector.X);
	AddMovementInput(RightDirection, MovementVector.Y);
	AddMovementInput(UpDirection, MovementVector.Z);
	/*FVector tmp = GetActorRotation().RotateVector(MovementVector);
	AddMovementInput(tmp, 1.f);*/

}
