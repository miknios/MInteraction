// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractorGatherers/MIInteractorGatherer_FirstPerson.h"

#include "MIInteractableFunctionLibrary.h"
#include "MIInteractableInterface.h"
#include "MIInteractorComponent.h"
#include "MInteractionSettings.h"
#include "MMath.h"
#include "GameFramework/Character.h"

static TAutoConsoleVariable<bool> CVarShowFirstPersonGathererDebugs(
	TEXT("m.Interaction.ShowFirstPersonGathererDebugs"), false, TEXT("Show First Person Gatherer Debugs"));

void UMIInteractorGatherer_FirstPerson::GatherInteractables(UMIInteractorComponent* InteractorComp,
                                                            TSet<UObject*>& OutInteractablesInRange,
                                                            TOptional<UObject*>& OutInteractableToFocus)
{
	if (ACharacter* Character = Cast<ACharacter>(InteractorComp->GetOwner()))
	{
		FVector AimLocation;
		FRotator AimRotator;
		Character->GetActorEyesViewPoint(AimLocation, AimRotator);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(InteractorComp->GetOwner());

		const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(OverlapRadius);
		TArray<FOverlapResult> OverlapResults;
		GetWorld()->OverlapMultiByProfile(OverlapResults, AimLocation, FQuat::Identity,
		                                  UMInteractionSettings::Get()->InteractableDetectionCollisionProfileName,
		                                  CollisionShape, QueryParams);

		FVector AimVector = AimRotator.Vector();

		float MatchResultBest = -1;
		for (auto OverlapResult : OverlapResults)
		{
			AActor* OverlapActor = OverlapResult.GetActor();
			if (UMIInteractableFunctionLibrary::IsInteractable(OverlapActor))
			{
				const FVector ComponentLocation = OverlapResult.Component->GetComponentLocation();
				const FVector ToComponentHitVector = MMath::FromToVectorNormalized(AimLocation, ComponentLocation);
				UObject* Interactable = UMIInteractableFunctionLibrary::GetInteractableFromActor(OverlapActor);
				OutInteractablesInRange.Add(Interactable);

				const float Distance = FVector::Dist(AimLocation, ComponentLocation);
				const float Dot = AimVector.Dot(ToComponentHitVector);

				if (IMIInteractableInterface::Execute_CanInteract(Interactable, InteractorComp->GetOwner())
					&& Distance <= FocusDistanceThreshold
					&& Dot > InteractableInRangeDotThreshold)
				{
					const float DistanceNormalized = Distance / OverlapRadius;
					const float MatchResult = (Dot * AngleMatchWeight + (1 - DistanceNormalized) * DistanceMatchWeight)
						/ (AngleMatchWeight + DistanceMatchWeight);

					if (MatchResult > MatchResultBest)
					{
						MatchResultBest = MatchResult;
						OutInteractableToFocus = Interactable;
					}
				}

				if (CVarShowFirstPersonGathererDebugs.GetValueOnGameThread())
				{
					DrawDebugLine(GetWorld(), AimLocation, ComponentLocation, FColor::White, false, 0);
					DrawDebugString(GetWorld(), ComponentLocation,
					                FString::Printf(TEXT("Distance = %d\nDot = %f"), FMath::FloorToInt(Distance), Dot),
					                nullptr, FColor::White, 0, true, 3);
				}
			}
		}
	}
}
