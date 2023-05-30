// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseButtonWidget.h"

static UBaseButtonWidget* TutorialButton;

bool UBaseButtonWidget::IsButtonInteractable() const
{
	if (!TutorialButton)
		return true;

	return TutorialButton == this;
}

void UBaseButtonWidget::SetTutorialButton(UBaseButtonWidget* NewTutorialButton)
{
	TutorialButton = NewTutorialButton;
}
