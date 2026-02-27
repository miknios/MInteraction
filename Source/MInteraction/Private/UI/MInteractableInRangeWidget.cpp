// Copyright (c) Miknios. All rights reserved.


#include "UI/MInteractableInRangeWidget.h"

bool UMInteractableInRangeWidget::IsFocused() const
{
	return bFocused;
}

void UMInteractableInRangeWidget::SetInteractable(const TScriptInterface<IMIInteractableInterface>& InInteractable)
{
	InteractableObject = InInteractable.GetObject();
}

TScriptInterface<IMIInteractableInterface> UMInteractableInRangeWidget::GetInteractable() const
{
	if (!InteractableObject.IsValid())
	{
		return nullptr;
	}

	return InteractableObject.Get();
}

void UMInteractableInRangeWidget::SetFocused(const bool bInFocused)
{
	if (bFocused == bInFocused)
	{
		return;
	}

	bFocused = bInFocused;
	OnFocusChanged(bFocused);
}

void UMInteractableInRangeWidget::OnRemoved_Implementation()
{
	OnRemoveFinished();
}

void UMInteractableInRangeWidget::OnRemoveFinished()
{
	OnRemovedPromise.SetValue();
}

SD::TExpectedFuture<void> UMInteractableInRangeWidget::RequestWidgetRemove()
{
	OnRemovedPromise = SD::TExpectedPromise<void>();

	OnRemoved();

	return OnRemovedPromise.GetFuture();
}
