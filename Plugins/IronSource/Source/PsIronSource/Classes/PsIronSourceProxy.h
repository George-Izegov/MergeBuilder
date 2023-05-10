// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#pragma once

#include <atomic>

#include "PsIronSourceProxy.generated.h"

//Comment it because we have problems with linux compilation
//Working with int codes

/*UENUM(BlueprintType)
enum class EIronSourceEventType : uint8
{
	ReceivedReward,    // 0 after the user has been rewarded 
	VideoShowFailed,   // 1 there is a problem playing the video
	VideoOpened,       // 2 when we take control, but before the video has started playing
	VideoClosed,       // 3 when we return control back to your hands
	VideoStarted,      // 4 video has started playing
	VideoEnded,        // 5 video has stopped playing
	VideoTapped,       // 6 video has been tapped
	VideoAvailable,    // 7 video has changed availability to Available
	VideoNotAvailable, // 8 video has changed availability to Not Available
	Impression         // 9 ad impression delegate callback
};*/

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPSIronSourceVideoDelegate, int32, Event);

USTRUCT()
struct FPsIronSourceImpressionData
{
	GENERATED_BODY()

	UPROPERTY()
	FString AuctionId;

	UPROPERTY()
	FString AdUnit;

	UPROPERTY()
	FString AdNetwork;

	UPROPERTY()
	FString InstanceName;

	UPROPERTY()
	FString InstanceId;

	UPROPERTY()
	FString Country;

	UPROPERTY()
	FString Placement;

	UPROPERTY()
	float Revenue;

	UPROPERTY()
	FString Precision;

	UPROPERTY()
	FString Ab;

	UPROPERTY()
	FString SegmentName;

	UPROPERTY()
	float LifetimeRevenue;

	UPROPERTY()
	FString EncryptedCpm;

	UPROPERTY()
	float ConversionValue;

	FPsIronSourceImpressionData()
		: Revenue(0.f)
		, LifetimeRevenue(0.f)
		, ConversionValue(0.f)
	{
	}
};

UCLASS()
class PSIRONSOURCE_API UPsIronSourceProxy : public UObject
{
	GENERATED_UCLASS_BODY()

public:
	/////////////////////////////////////////////////////////////////////////
	// Setup

	/** Initialize */
	UFUNCTION(BlueprintCallable, Category = "IronSource")
	virtual void InitIronSource(const FString& UserId);

	/** Update userid after change account */
	UFUNCTION(BlueprintCallable, Category = "IronSource")
	virtual void ForceUpdateIronSourceUser(const FString& UserId);

	/** Whether SDK is initialized */
	UFUNCTION(BlueprintCallable, Category = "IronSource")
	bool IsInitialized() const;

	/** Determine if a locally cached rewarded video exists on the mediation level */
	UFUNCTION(BlueprintCallable, Category = "IronSource")
	virtual bool HasRewardedVideo() const;

	/** Get reward name for placement */
	UFUNCTION(BlueprintCallable, Category = "IronSource")
	virtual FString GetPlacementRewardName(const FString& PlacementName) const;

	/** Get reward amount for placement */
	UFUNCTION(BlueprintCallable, Category = "IronSource")
	virtual FString GetPlacementRewardAmount(const FString& PlacementName) const;

	/** Verify if a certain placement has reached its ad limit */
	UFUNCTION(BlueprintCallable, Category = "IronSource")
	virtual bool IsRewardedVideoCappedForPlacement(const FString& PlacementName) const;

	/** Show rewarded video */
	UFUNCTION(BlueprintCallable, Category = "IronSource")
	virtual void ShowRewardedVideo(const FString& PlacementName, const FString& ParamKey, const FString& ParamValue) const;

	/** Set GDPR consent status */
	UFUNCTION(BlueprintCallable, Category = "IronSource")
	virtual void SetGDPRConsent(bool bConsent) const;

	/** Whether there are events waiting to be sent to game thread */
	bool HasQueuedEvents() const;

	/** Increment event counter */
	void EnqueueEvent();

	/** Decrement event counter */
	void DequeueEvent();

	/** Set impression data */
	void SetImpressionData(const FPsIronSourceImpressionData& InImpressionData);

	/** Get last impression data */
	FPsIronSourceImpressionData GetImpressionData() const;

public:
	/** Delegate broadcasting video-related events */
	UPROPERTY(BlueprintAssignable)
	FPSIronSourceVideoDelegate VideoStateDelegate;

protected:
	/** Whether SDK is initialized */
	bool bIronSourceInitialized;

	/** Number of queued events */
	std::atomic<int32> QueuedEventsCount;

	/** Impression data instance */
	FPsIronSourceImpressionData ImpressionData;
};