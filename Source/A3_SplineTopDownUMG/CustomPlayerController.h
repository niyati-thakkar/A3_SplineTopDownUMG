// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "WallGenerator.h"
#include "CustomPlayerController.generated.h"

/**
 * 
 */

DECLARE_DELEGATE_TwoParams(PrintLog,const FString& Message, const FLinearColor& ColorOpacity)

UCLASS()
class A3_SPLINETOPDOWNUMG_API ACustomPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ACustomPlayerController();

	UPROPERTY()
	AWallGenerator* UserWall;

	void BeginPlay();

protected:
	void SetupInputComponent() override;
	void GetLocation();

public:
	PrintLog DelegateLog;

	UFUNCTION(BlueprintImplementableEvent)
	void LogMessage(const FString& Message, const FLinearColor& ColorOpacity);
	
	UFUNCTION(BlueprintCallable)
	void DestroyWallMesh();

	UFUNCTION(BlueprintCallable)
	void DestroyWallComponent();

	UFUNCTION(BlueprintCallable)
	void DestroyAllWalls();

	UFUNCTION(BlueprintCallable)
	void GenerateNewWallComponent();
};
