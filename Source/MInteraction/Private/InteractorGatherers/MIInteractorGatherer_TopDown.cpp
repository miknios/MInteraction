// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractorGatherers/MIInteractorGatherer_TopDown.h"

#include "MIInteractableFunctionLibrary.h"
#include "MIInteractableInterface.h"
#include "MInteractionSettings.h"
#include "MMath.h"

static TAutoConsoleVariable<bool> CVarShowInteractionArea(
	TEXT("m.Interaction.ShowInteractionArea"), false, TEXT("Show Interaction Area Gizmo"), ECVF_Default);

void UMIInteractorGatherer_TopDown::GatherInteractables(UMIInteractorComponent* InteractorComp,
                                                                       TSet<UObject*>& OutInteractablesInRange,
                                                                       TOptional<UObject*>& OutInteractableToFocus)
{
	auto CalculateInteractableMatchResult = [this](const AActor* Owner,
	                                               const TWeakObjectPtr<UPrimitiveComponent> ComponentHit,
	                                               const FVector& OverlapPos) -> float
	{
		FVector OwnerForward = Owner->GetActorForwardVector();
		OwnerForward.Z = 0;
		OwnerForward.Normalize();

		FVector ToComponentHitVector = MMath::FromToVector(Owner->GetActorLocation(),
		                                                   ComponentHit->GetComponentLocation());
		ToComponentHitVector.Z = 0;
		ToComponentHitVector.Normalize();

		const float Dot = OwnerForward.Dot(ToComponentHitVector);
		const float DistanceNormalized = FVector::Dist(OverlapPos, ComponentHit->GetComponentLocation())
			/ OverlapRadius;

		const float MatchResult = (Dot * AngleMatchWeight + DistanceNormalized * DistanceMatchWeight)
			/ (AngleMatchWeight + DistanceMatchWeight);

		return MatchResult;
	};

	AActor* Owner = InteractorComp->GetOwner();
	const FVector OverlapPos = Owner->GetActorLocation() + OverlapOffsetFromRoot;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);

	const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(OverlapRadius, OverlapHalfHeight);

	if (CVarShowInteractionArea.GetValueOnGameThread())
	{
		DrawDebugCapsule(GetWorld(), OverlapPos, CollisionShape.Capsule.HalfHeight, CollisionShape.Capsule.Radius,
		                 FQuat::Identity, FColor::White);
	}

	TArray<FOverlapResult> OverlapResults;
	GetWorld()->OverlapMultiByProfile(OverlapResults, OverlapPos, FQuat::Identity,
	                                  UMInteractionSettings::Get()->InteractableDetectionCollisionProfileName,
	                                  CollisionShape, QueryParams);


	float MatchResultBest = -1;
	for (auto OverlapResult : OverlapResults)
	{
		if (UMIInteractableFunctionLibrary::IsInteractable(OverlapResult.GetActor()))
		{
			UObject* Interactable = UMIInteractableFunctionLibrary::GetInteractableFromActor(OverlapResult.GetActor());
			OutInteractablesInRange.Add(Interactable);

			if (IMIInteractableInterface::Execute_CanInteract(Interactable, Owner))
			{
				const float MatchResult = CalculateInteractableMatchResult(Owner, OverlapResult.Component, OverlapPos);
				if (MatchResult > MatchResultBest)
				{
					MatchResultBest = MatchResult;
					OutInteractableToFocus = Interactable;
				}
			}
		}
	}
}
