// Copyright (c) 2021 Nineva Studios

#pragma once

#include "UObject/Object.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGMethodCallUtils.h"
#endif

#if PLATFORM_IOS
#import <FirebaseFirestore/FirebaseFirestore.h>
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "firebase/firestore.h"
#endif

#include "FGListenerRegistration.generated.h"

/**
* Represents a listener that can be removed by calling remove.
*/
UCLASS(BlueprintType)
class FIREBASEGOODIES_API UFGListenerRegistration : public UObject
{
	GENERATED_BODY()

	virtual ~UFGListenerRegistration();

public:

	/**
	* Removes the listener being tracked by this UFGListenerRegistration. After the initial call, subsequent calls have no effect.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	void Remove();

	/**
	 * Convenience function to check whether the native object is valid.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Firestore")
	bool IsListenerRegistrationValid();

#if PLATFORM_ANDROID
	void Init(jobject javaListenerRegistration);
	jobject JavaListenerRegistrationObject;
#endif
	
#if PLATFORM_IOS
	void Init(id<FIRListenerRegistration> iosListenerRegistration);
	id<FIRListenerRegistration> ListenerRegistrationObject;
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	void Init(firebase::firestore::ListenerRegistration listenerRegistration);
	firebase::firestore::ListenerRegistration ListenerRegistration;
#endif
};
