// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MIInteractableInterface.generated.h"

UINTERFACE(Blueprintable)
class UMIInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for things that player can interact with common interaction button
 */
class MINTERACTION_API IMIInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool Interact(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanInteract(AActor* Instigator);
	
	virtual bool CanInteract_Implementation(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EnableInteractableFocused();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DisableInteractableFocused();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsFocused();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EnableInteraction();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void DisableInteraction();

	UFUNCTION(BlueprintNativeEvent)
	FText GetDisplayName();

	UFUNCTION(BlueprintNativeEvent)
	FText GetInteractionVerb();

	UFUNCTION(BlueprintNativeEvent)
	FVector GetObjectNameAnchorPos();
};
