// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MIInteractorComponent.generated.h"

class UMPointSelectorQuerySettingsAsset;
class UMIInteractorGatherer_Base;
class UWidget;
class IMIInteractableInterface;

/**
 * Component attached to player pawn
 * 
 * Finds interactables in range and finds the most desired one to call Interact on when the button is pressed
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MINTERACTION_API UMIInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMIInteractorComponent();

	// UActorComponent
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// ~ UActorComponent

	UFUNCTION(BlueprintCallable, Category = "Interactor")
	void Interact();

	UFUNCTION(Server, Reliable)
	void ServerRPCInteract(UObject* Interactable);

	TArrayView<const TScriptInterface<IMIInteractableInterface>> GetInteractablesInRange() const;
	const TScriptInterface<IMIInteractableInterface>& GetInteractableFocused() const;

protected:
	/**
	 *	Additional functionality when new interactable is in focus should be implemented in game module by extending this
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactor")
	void OnInteractableFocusBegin(const TScriptInterface<IMIInteractableInterface>& Interactable);
	virtual void OnInteractableFocusBegin_Implementation(const TScriptInterface<IMIInteractableInterface>& Interactable);

	/**
	 *	Additional functionality when interactable focus is lost should be implemented in game module by extending this
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactor")
	void OnInteractableFocusLost(const TScriptInterface<IMIInteractableInterface>& Interactable);
	virtual void OnInteractableFocusLost_Implementation(const TScriptInterface<IMIInteractableInterface>& Interactable);

	UPROPERTY(EditAnywhere, Category = "Interactor")
	TObjectPtr<UMPointSelectorQuerySettingsAsset> QuerySettingsAsset;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = "Interactor")
	TScriptInterface<IMIInteractableInterface> InteractableFocused;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = "Interactor")
	TArray<TScriptInterface<IMIInteractableInterface>> InteractablesInRange;

private:
	void GatherInteractablesInRange();
};
