// Copyright (c) Miknios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ExpectedFuture.h"
#include "MIInteractableInterface.h"
#include "Blueprint/UserWidget.h"
#include "MInteractableInRangeWidget.generated.h"

/**
 * Widget displayed on top of interactable object when it's in range of @UMInteractorComponent
 * 
 * Be mindful that these widgets are pooled by that component
 */
UCLASS(Abstract)
class MINTERACTION_API UMInteractableInRangeWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class UMInteractorWidgetComponent;

public:
	UFUNCTION(BlueprintCallable)
	bool IsFocused() const;

	void SetInteractable(const TScriptInterface<IMIInteractableInterface>& InInteractable);
	TScriptInterface<IMIInteractableInterface> GetInteractable() const;
	void SetFocused(bool bInFocused);

protected:
	UPROPERTY()
	TWeakObjectPtr<UObject> InteractableObject;

	UFUNCTION(BlueprintImplementableEvent)
	void OnCreated();

	UFUNCTION(BlueprintImplementableEvent)
	void OnFocusChanged(bool bFocusedNew);

	// This function should call @OnRemoveFinished at some point (eg. after playing some animation) so it can trigger poolin on the component side
	UFUNCTION(BlueprintNativeEvent)
	void OnRemoved();

	UFUNCTION(BlueprintCallable)
	void OnRemoveFinished();

	SD::TExpectedFuture<void> RequestWidgetRemove();

private:
	bool bFocused = false;
	SD::TExpectedPromise<void> OnRemovedPromise;
};
