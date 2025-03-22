// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MIInteractorComponent.generated.h"

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

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Interactor")
	void Interact();

	UFUNCTION(Server, Reliable)
	void ServerRPCInteract(UObject* Interactable);

protected:
	/**
	 *	Additional functionality when new interactable is in focus should be implemented in game module by extending this
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactor")
	void OnInteractableFocusBegin(UObject* Interactable);
	virtual void OnInteractableFocusBegin_Implementation(UObject* Interactable);

	/**
	 *	Additional functionality when interactable focus is lost should be implemented in game module by extending this
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interactor")
	void OnInteractableFocusLost(UObject* Interactable);
	virtual void OnInteractableFocusLost_Implementation(UObject* Interactable);

	void GatherInteractablesInRange();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Interactor")
	TSubclassOf<UMIInteractorGatherer_Base> InteractorGathererClass;

	UPROPERTY(VisibleInstanceOnly, Category = "Interactor")
	UObject* InteractableFocused;

	UPROPERTY()
	UMIInteractorGatherer_Base* InteractorGathererInstance;
};
