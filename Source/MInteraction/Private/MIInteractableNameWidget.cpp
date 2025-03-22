// Fill out your copyright notice in the Description page of Project Settings.


#include "MIInteractableNameWidget.h"
#include "Components/TextBlock.h"

void UMIInteractableNameWidget::SetObjectName(FText ObjectName)
{
	ObjectNameTextBlock->SetText(ObjectName);
}

void UMIInteractableNameWidget::SetFocusedState_Implementation(bool bFocused)
{
}

void UMIInteractableNameWidget::StartDisplaying_Implementation()
{
}
