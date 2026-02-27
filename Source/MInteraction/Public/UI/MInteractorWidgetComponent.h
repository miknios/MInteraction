// Copyright (c) Miknios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MInteractorWidgetComponent.generated.h"

class UMInteractableInRangeWidget;
class UMIInteractorComponent;

/*
 * When this component is placed on an actor it's location is used to determine where to draw the interactable widget when it's inside the interactor's interaction range
 * 
 * There should be only one of these components on a single interactable actor
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MINTERACTION_API UMInteractableWidgetSocketComponent : public USceneComponent
{
	GENERATED_BODY()
};

/*
 * Component that takes care of drawing widgets of interactables in range. 
 * 
 * It is separated from UMIInteractorComponent to make it easier to extend functionality of interactor without dealing with widget-related code
 * and to make it easier to extend widget-related functionality without dealing with interactor-related code. 
 * For example, you can create a new component that extends UMIInteractorWidgetComponent and add a new widget for interactables in range without changing anything in an interactor component.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MINTERACTION_API UMInteractorWidgetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMInteractorWidgetComponent();

	// UActorComponent
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	// ~ UActorComponent

private:
	static FVector GetWidgetWorldLocation(UObject& InteractableObject);
	
	void UpdateWidgets();
	void UpdateWidgetScreenPosition(UObject& InteractableObject, UMInteractableInRangeWidget& Widget);
	void AcquireWidget(UObject& InteractableObject);
	void ReleaseWidget(UObject& InteractableObject);
	UMInteractableInRangeWidget* GetPooledWidget();
	void ReturnToPool(UMInteractableInRangeWidget& Widget);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMInteractableInRangeWidget> InteractableInRangeClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI", AdvancedDisplay)
	int32 InitialPoolSize = 10;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = "UI")
	TObjectPtr<UMIInteractorComponent> Interactor;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = "UI")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = "UI")
	TMap<TWeakObjectPtr<UObject>, UMInteractableInRangeWidget*> WidgetForInteractableObject;

	UPROPERTY(Transient, VisibleInstanceOnly, Category = "UI")
	TArray<TObjectPtr<UMInteractableInRangeWidget>> WidgetPool;
};
