// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameModeBase.h"


ACustomGameModeBase::ACustomGameModeBase()
{
	DefaultPawnClass = ATopDownCameraPawn::StaticClass();
	PlayerControllerClass = ACustomPlayerController::StaticClass();

}
