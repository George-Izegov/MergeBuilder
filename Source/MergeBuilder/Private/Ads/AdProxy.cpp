// Fill out your copyright notice in the Description page of Project Settings.


#include "Ads/AdProxy.h"
#include "AudioMixerDevice.h"

void UAdProxy::Init(const FString& UserId, bool bAgreeGDPR)
{
}

void UAdProxy::ShowRewardedVideo(const FString& RewardedVideoParam, EAdPlacementType PlacementType)
{
	AdStarted = true;
	CurrentAdPlacement = PlacementType;
	CurrentParam = RewardedVideoParam;
}

void UAdProxy::ShowRewardedVideoCallback(int32 EventType)
{
	if (EventType == 0)
	{
		OnRewardedAdSuccessWatched.Broadcast(CurrentAdPlacement, CurrentParam);
		// TODO: Send analytic
	}
	
	// video started
	if (EventType == 2)
	{
		Audio::FMixerDevice* MixerDevice = static_cast<Audio::FMixerDevice*>(GetWorld()->GetAudioDeviceRaw());
		MixerDevice->SuspendContext();
	}

	// video closed
	if (EventType == 3)
	{
		AdStarted = false;
		
		Audio::FMixerDevice* MixerDevice = static_cast<Audio::FMixerDevice*>(GetWorld()->GetAudioDeviceRaw());
		MixerDevice->ResumeContext();

		OnRewardedAdClosed.Broadcast();
	}
	
	if (EventType == 1)
	{
		AdStarted = false;
		
		// TODO: Show no ad window
	}
}
