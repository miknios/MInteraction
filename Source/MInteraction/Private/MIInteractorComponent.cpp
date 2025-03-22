// Fill out your copyright notice in the Description page of Project Settings.


#include "MIInteractorComponent.h"
#include "DrawDebugHelpers.h"
#include "MIInteractableComponent.h"
#include "MIInteractableNameWidgetSystem.h"
#include "InteractorGatherers/MIInteractorGatherer_Base.h"
#include "Kismet/GameplayStatics.h"

UMIInteractorComponent::UMIInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMIInteractorComponent::BeginPlay()
{
	Super::BeginPlay();

	InteractorGathererInstance = NewObject<UMIInteractorGatherer_Base>(this, InteractorGathererClass);
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
		ServerRPCInteract(InteractableFocused);
	}
}

void UMIInteractorComponent::ServerRPCInteract_Implementation(UObject* Interactable)
{
	IMIInteractableInterface::Execute_Interact(Interactable, GetOwner());
}

void UMIInteractorComponent::OnInteractableFocusBegin_Implementation(UObject* Interactable)
{
}

void UMIInteractorComponent::OnInteractableFocusLost_Implementation(UObject* Interactable)
{
}

void UMIInteractorComponent::GatherInteractablesInRange()
{
	if (!InteractorGathererInstance)
		return;

	if (GetOwnerRole() < ROLE_AutonomousProxy)
		return;

	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
		return;

	const APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (!PlayerController)
		return;

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
		return;

	TSet<UObject*> InteractablesInRange;
	TOptional<UObject*> InteractableToFocus;
	InteractorGathererInstance->GatherInteractables(this, InteractablesInRange, InteractableToFocus);

	UMIInteractableNameWidgetSystem::Get(LocalPlayer)
		->SetInteractablesInRange(InteractablesInRange);

	if (InteractableToFocus.IsSet())
	{
		if (InteractableFocused != InteractableToFocus)
		{
			if (InteractableFocused != nullptr)
			{
				IMIInteractableInterface::Execute_DisableInteractableFocused(InteractableFocused);

				OnInteractableFocusLost(InteractableFocused);
			}

			InteractableFocused = InteractableToFocus.GetValue();
			IMIInteractableInterface::Execute_EnableInteractableFocused(InteractableToFocus.GetValue());

			OnInteractableFocusBegin(InteractableToFocus.GetValue());
		}
	}
	else
	{
		if (InteractableFocused != nullptr)
		{
			IMIInteractableInterface::Execute_DisableInteractableFocused(InteractableFocused);
			InteractableFocused = nullptr;

			OnInteractableFocusLost(InteractableFocused);
		}
	}
}
