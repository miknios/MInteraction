// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MIInteractableFunctionLibrary.generated.h"

class UMIInteractableComponent;

/**
 * 
 */
UCLASS(meta=(ScriptName="InteractableLibrary"))
class MINTERACTION_API UMIInteractableFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static bool IsInteractable(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	static bool IsObjectInteractable(UObject* Object);

	UFUNCTION(BlueprintCallable)
	static bool Interact(AActor* Actor);

	UFUNCTION(BlueprintCallable)
	static UObject* GetInteractableFromActor(AActor* Actor);
};
