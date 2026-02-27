// Fill out your copyright notice in the Description page of Project Settings.


#include "MIInteractorComponent.h"

#include "DrawDebugHelpers.h"
#include "MIInteractableComponent.h"
#include "MIInteractableFunctionLibrary.h"
#include "MPointSelectorQueryLibrary.h"

UMIInteractorComponent::UMIInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMIInteractorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UMIInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	GatherInteractablesInRange();
}

void UMIInteractorComponent::Interact()
{
	if (InteractableFocused)
	{
		ServerRPCInteract(InteractableFocused.GetObject());
	}
}

TArrayView<const TScriptInterface<IMIInteractableInterface>> UMIInteractorComponent::GetInteractablesInRange() const
{
	return InteractablesInRange;
}

const TScriptInterface<IMIInteractableInterface>& UMIInteractorComponent::GetInteractableFocused() const
{
	return InteractableFocused;
}

void UMIInteractorComponent::ServerRPCInteract_Implementation(UObject* Interactable)
{
	IMIInteractableInterface::Execute_Interact(Interactable, GetOwner());
}

void UMIInteractorComponent::OnInteractableFocusBegin_Implementation(const TScriptInterface<IMIInteractableInterface>& Interactable)
{
}

void UMIInteractorComponent::OnInteractableFocusLost_Implementation(const TScriptInterface<IMIInteractableInterface>& Interactable)
{
}

void UMIInteractorComponent::GatherInteractablesInRange()
{
	if (GetOwnerRole() < ROLE_AutonomousProxy)
	{
		return;
	}

	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (!PlayerController)
	{
		return;
	}

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	FVector ReferenceLocation;
	FRotator EyesViewPointRot;
	Pawn->GetActorEyesViewPoint(ReferenceLocation, EyesViewPointRot);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Pawn);

	const TArray<FMPointSelectorQueryScoredPointData> Points = UMPointSelectorQueryLibrary::QueryPoints(
		this, ReferenceLocation, Pawn->GetActorForwardVector(), QuerySettingsAsset, QueryParams);

	InteractablesInRange.Empty();

	// Update interactables in range
	Algo::Transform(
		Points.FilterByPredicate([](const FMPointSelectorQueryScoredPointData& Point)
		{
			const AActor* LookAtTargetActor = IMSelectorQueryPointInterface::Execute_GetActorOwner(Point.LookAtPoint.GetObject());
			if (LookAtTargetActor == nullptr)
			{
				return false;
			}

			const bool bInteractable = UMIInteractableFunctionLibrary::IsActorInteractable(LookAtTargetActor);
			return bInteractable;
		}),
		InteractablesInRange,
		[](const FMPointSelectorQueryScoredPointData& Point)
		{
			const AActor* LookAtTargetActor = IMSelectorQueryPointInterface::Execute_GetActorOwner(Point.LookAtPoint.GetObject());
			TScriptInterface<IMIInteractableInterface> Interactable =
				UMIInteractableFunctionLibrary::GetInteractableFromActor(LookAtTargetActor);
			return MoveTemp(Interactable);
		});

	// If there's any interactable in range focus the first one - it should be the one that has the highest score
	TOptional<TScriptInterface<IMIInteractableInterface>> InteractableToFocus =
		InteractablesInRange.Num() > 0
			? TOptional(InteractablesInRange[0])
			: NullOpt;

	if (InteractableToFocus.IsSet() && InteractableFocused != InteractableToFocus.GetValue())
	{
		if (InteractableFocused != nullptr)
		{
			IMIInteractableInterface::Execute_DisableInteractableFocused(InteractableFocused.GetObject());
			OnInteractableFocusLost(InteractableFocused);
		}

		InteractableFocused = InteractableToFocus.GetValue();
		IMIInteractableInterface::Execute_EnableInteractableFocused(InteractableFocused.GetObject());
		OnInteractableFocusBegin(InteractableFocused);
	}
	else if (!InteractableToFocus.IsSet() && InteractableFocused != nullptr)
	{
		IMIInteractableInterface::Execute_DisableInteractableFocused(InteractableFocused.GetObject());
		OnInteractableFocusLost(InteractableFocused);
		InteractableFocused = nullptr;
	}
}
