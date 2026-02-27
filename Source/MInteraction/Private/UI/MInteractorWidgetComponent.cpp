// Copyright (c) Miknios. All rights reserved.


#include "UI/MInteractorWidgetComponent.h"

#include "MDebug.h"
#include "MIInteractableFunctionLibrary.h"
#include "MIInteractorComponent.h"
#include "MInteraction.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MInteractableInRangeWidget.h"
#include "ExpectedFuture.h"
#include "FutureExtensionTaskGraph.h"

UMInteractorWidgetComponent::UMInteractorWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_PostUpdateWork;
}

void UMInteractorWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	const AActor* Owner = GetOwner();
	if (!ensureAlways(IsValid(Owner)))
	{
		return;
	}

	Interactor = Owner->FindComponentByClass<UMIInteractorComponent>();
	if (!IsValid(Interactor))
	{
		SetComponentTickEnabled(false);
		return;
	}

	PlayerController = Cast<APlayerController>(Owner->GetInstigatorController());
	if (!IsValid(PlayerController))
	{
		M::Debug::LogUserError(LogMInteraction, TEXT("Interactor Widget Component must be attached to a Player Controlled Pawn"), Owner);
		SetComponentTickEnabled(false);
		return;
	}

	if (InteractableInRangeClass == nullptr)
	{
		M::Debug::LogUserError(LogMInteraction, TEXT("InteractableInRangeClass must be set"), Owner);
		SetComponentTickEnabled(false);
		return;
	}

	// Prewarm pool
	for (int32 i = 0; i < InitialPoolSize; ++i)
	{
		UMInteractableInRangeWidget* Widget = CreateWidget<UMInteractableInRangeWidget>(PlayerController, InteractableInRangeClass);
		Widget->SetVisibility(ESlateVisibility::Collapsed);
		WidgetPool.Add(Widget);
	}
}

void UMInteractorWidgetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UMInteractorWidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsValid(Interactor) || !IsValid(PlayerController))
	{
		return;
	}

	UpdateWidgets();
}

FVector UMInteractorWidgetComponent::GetWidgetWorldLocation(UObject& InteractableObject)
{
	if (!ensureAlways(UMIInteractableFunctionLibrary::IsObjectInteractable(&InteractableObject)))
	{
		return FVector::ZeroVector;
	}

	const AActor* Actor = UMIInteractableFunctionLibrary::GetActorFromInteractable(&InteractableObject);
	if (!ensureAlways(IsValid(Actor)))
	{
		return FVector::ZeroVector;
	}

	if (const UMInteractableWidgetSocketComponent* SocketComponent = Actor->FindComponentByClass<UMInteractableWidgetSocketComponent>())
	{
		return SocketComponent->GetComponentLocation();
	}

	return Actor->GetActorLocation();
}

void UMInteractorWidgetComponent::UpdateWidgets()
{
	const TArrayView<const TScriptInterface<IMIInteractableInterface>> InteractablesInRange = Interactor->GetInteractablesInRange();
	const TScriptInterface<IMIInteractableInterface> InteractableFocused = Interactor->GetInteractableFocused();

	TSet<const UObject*> InteractableObjectsInRangeSet;
	Algo::Transform(InteractablesInRange, InteractableObjectsInRangeSet, [](const TScriptInterface<IMIInteractableInterface>& Interactable)
	{
		return Interactable.GetObject();
	});

	for (TScriptInterface Interactable : InteractablesInRange)
	{
		UObject* InteractableObject = Interactable.GetObject();
		if (InteractableObject == nullptr)
		{
			continue;
		}

		if (!WidgetForInteractableObject.Contains(InteractableObject))
		{
			AcquireWidget(*InteractableObject);
		}
	}

	TArray<TWeakObjectPtr<>> InteractableObjectsNotInRangeNew;

	for (auto [InteractableObject, Widget] : WidgetForInteractableObject)
	{
		if (!InteractableObject.IsValid())
		{
			continue;
		}

		if (!InteractableObjectsInRangeSet.Contains(InteractableObject.Get()))
		{
			InteractableObjectsNotInRangeNew.Add(InteractableObject);
		}
	}

	for (TWeakObjectPtr<>& InteractableObject : InteractableObjectsNotInRangeNew)
	{
		ReleaseWidget(*InteractableObject.Get());
	}

	for (auto [InteractableObject, Widget] : WidgetForInteractableObject)
	{
		if (!InteractableObject.IsValid())
		{
			continue;
		}

		if (!IsValid(Widget))
		{
			continue;
		}

		UpdateWidgetScreenPosition(*InteractableObject, *Widget);
		Widget->SetFocused(InteractableObject.Get() == InteractableFocused.GetObject());
	}
}

void UMInteractorWidgetComponent::UpdateWidgetScreenPosition(UObject& InteractableObject, UMInteractableInRangeWidget& Widget)
{
	const FVector WorldLocation = GetWidgetWorldLocation(InteractableObject);

	FVector2D ScreenPos;
	const bool bProjected =
		UGameplayStatics::ProjectWorldToScreen(
			PlayerController,
			WorldLocation,
			ScreenPos,
			true);

	if (!bProjected)
	{
		Widget.SetVisibility(ESlateVisibility::Hidden);
		return;
	}

	Widget.SetVisibility(
		ESlateVisibility::HitTestInvisible);

	Widget.SetPositionInViewport(ScreenPos, true);
	Widget.SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
}

void UMInteractorWidgetComponent::AcquireWidget(UObject& InteractableObject)
{
	UMInteractableInRangeWidget* Widget = GetPooledWidget();

	if (Widget == nullptr)
	{
		Widget = CreateWidget<UMInteractableInRangeWidget>(PlayerController, InteractableInRangeClass);
	}

	Widget->SetInteractable(&InteractableObject);
	Widget->AddToPlayerScreen(0);

	WidgetForInteractableObject.Emplace(&InteractableObject, Widget);
}

void UMInteractorWidgetComponent::ReleaseWidget(UObject& InteractableObject)
{
	UMInteractableInRangeWidget** WidgetFoundPtrPtr = WidgetForInteractableObject.Find(&InteractableObject);
	if (WidgetFoundPtrPtr == nullptr)
	{
		return;
	}

	UMInteractableInRangeWidget* WidgetFoundPtr = *WidgetFoundPtrPtr;

	if (WidgetFoundPtr)
	{
		WidgetFoundPtr->RequestWidgetRemove()
		              .Then(this, [this, WeakWidget = TWeakObjectPtr(WidgetFoundPtr)](SD::TExpected<void> ExpectedResult)
		              {
			              if (!WeakWidget.IsValid())
			              {
				              return;
			              }

			              ReturnToPool(*WeakWidget);
		              });
	}

	WidgetForInteractableObject.Remove(&InteractableObject);
}

UMInteractableInRangeWidget* UMInteractorWidgetComponent::GetPooledWidget()
{
	return WidgetPool.Num() > 0
		       ? WidgetPool.Pop()
		       : nullptr;
}

void UMInteractorWidgetComponent::ReturnToPool(UMInteractableInRangeWidget& Widget)
{
	Widget.RemoveFromParent();
	Widget.SetInteractable(nullptr);
	Widget.SetVisibility(ESlateVisibility::Collapsed);

	WidgetPool.Add(&Widget);
}
