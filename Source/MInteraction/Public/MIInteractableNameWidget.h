// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MIInteractableNameWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS(Blueprintable)
class MINTERACTION_API UMIInteractableNameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetObjectName(FText ObjectName);

	UFUNCTION(BlueprintNativeEvent)
	void SetFocusedState(bool bFocused);

	UFUNCTION(BlueprintNativeEvent)
	void StartDisplaying();

public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ObjectNameTextBlock;
};
