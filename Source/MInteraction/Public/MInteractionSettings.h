// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "MInteractionSettings.generated.h"

class UMIInteractableNameWidget;
/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="MInteraction Settings"))
class MINTERACTION_API UMInteractionSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	static UMInteractionSettings* Get() { return CastChecked<UMInteractionSettings>(StaticClass()->GetDefaultObject()); }

public:
	UPROPERTY(EditAnywhere, Config)
	FName InteractableDetectionCollisionProfileName;

	UPROPERTY(EditAnywhere, Config, Category = "Widget References")
	TSoftClassPtr<UMIInteractableNameWidget> InteractableNameWidgetClass;
};
