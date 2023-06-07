// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/BaseButtonWidget.h"
#include "Tutorial/MBTutorialSubsystem.h"

bool UBaseButtonWidget::IsButtonInteractable() const
{
	if (!UMBTutorialSubsystem::GetTutorialObject())
		return true;

	return UMBTutorialSubsystem::GetTutorialObject() == this;
}
