#include "Firestore/FGListenerRegistration.h"
#include "FirebaseGoodiesLog.h"

UFGListenerRegistration::~UFGListenerRegistration()
{
	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGListenerRegistration::~UFGListenerRegistration"));

	if (!IsListenerRegistrationValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	Env->DeleteGlobalRef(JavaListenerRegistrationObject);
	JavaListenerRegistrationObject = nullptr;
#elif PLATFORM_IOS
	(id<FIRListenerRegistration>)CFBridgingRelease(ListenerRegistrationObject);
	ListenerRegistrationObject = nil;
#endif
}

void UFGListenerRegistration::Remove()
{
UE_LOG(LogFirebaseGoodies, Verbose, TEXT("UFGListenerRegistration::Remove"));

	if (!IsListenerRegistrationValid())
	{
		return;
	}

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallVoidMethod(JavaListenerRegistrationObject, "remove", "()V");
#elif PLATFORM_IOS
	[ListenerRegistrationObject remove];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	ListenerRegistration.Remove();
#endif
}

bool UFGListenerRegistration::IsListenerRegistrationValid()
{
	bool Result = false;

#if PLATFORM_ANDROID
	Result = JavaListenerRegistrationObject != nullptr;
#elif PLATFORM_IOS
	Result = ListenerRegistrationObject != nil;
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	Result = true;
#endif
#if !WITH_EDITOR
	if (!Result)
	{
		UE_LOG(LogFirebaseGoodies, Error, TEXT("Native object is not valid."));
	}
#endif
	return Result;
}

#if PLATFORM_ANDROID
void UFGListenerRegistration::Init(jobject javaListenerRegistration)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	JavaListenerRegistrationObject = Env->NewGlobalRef(javaListenerRegistration);
}
#endif

#if PLATFORM_IOS
void UFGListenerRegistration::Init(id<FIRListenerRegistration> iosListenerRegistration)
{
	ListenerRegistrationObject = (id<FIRListenerRegistration>)CFBridgingRetain(iosListenerRegistration);
}
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
void UFGListenerRegistration::Init(firebase::firestore::ListenerRegistration listenerRegistration)
{
	ListenerRegistration = listenerRegistration;
}
#endif
