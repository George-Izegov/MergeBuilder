// Copyright (c) 2022 Nineva Studios

#include "CloudStorage/FGCloudStorage.h"

#include "Async/Async.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "FirebaseGoodiesLog.h"

#if PLATFORM_ANDROID
#include "Android/Utils/FGJavaConvertor.h"
#include "Android/Utils/FGMethodCallUtils.h"
#endif

#if PLATFORM_IOS
#import <FirebaseStorage/FirebaseStorage.h>
#endif

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "firebase/storage.h"
#include "firebase/app.h"

#include "Desktop/Storage/FGStorageOperationListener.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

#include "Desktop/FGUtils.h"
#endif

FCloudStorageIntDelegate UFGCloudStorage::OnUploadFromDataInMemoryProgressCallback;
FCloudStorageVoidDelegate UFGCloudStorage::OnUploadFromDataInMemoryCompleteCallback;
FCloudStorageStringDelegate UFGCloudStorage::OnUploadFromDataInMemoryFailureCallback;

FCloudStorageIntDelegate UFGCloudStorage::OnUploadFromLocalFileProgressCallback;
FCloudStorageVoidDelegate UFGCloudStorage::OnUploadFromLocalFileCompleteCallback;
FCloudStorageStringDelegate UFGCloudStorage::OnUploadFromLocalFileFailureCallback;

FCloudStorageStringDelegate UFGCloudStorage::OnGetDownloadUrlCompleteCallback;
FCloudStorageStringDelegate UFGCloudStorage::OnGetDownloadUrlFailureCallback;

FCloudStorageStringDelegate UFGCloudStorage::OnDownloadToLocalFileCompleteCallback;
FCloudStorageStringDelegate UFGCloudStorage::OnDownloadToLocalFileErrorCallback;
FCloudStorageIntDelegate UFGCloudStorage::OnDownloadToLocalFileProgressCallback;

FCloudStorageVoidDelegate UFGCloudStorage::OnDeleteFileCompleteCallback;
FCloudStorageStringDelegate UFGCloudStorage::OnDeleteFileErrorCallback;

FCloudStorageByteArrayDelegate UFGCloudStorage::OnDownloadInMemoryCompleteCallback;
FCloudStorageStringDelegate UFGCloudStorage::OnDownloadInMemoryErrorCallback;
FCloudStorageIntDelegate UFGCloudStorage::OnDownloadInMemoryProgressCallback;

FCloudStorageMetadataDelegate UFGCloudStorage::OnGetMetadataCompleteCallback;
FCloudStorageStringDelegate UFGCloudStorage::OnGetMetadataErrorCallback;

FCloudStorageMetadataDelegate UFGCloudStorage::OnUpdateMetadataCompleteCallback;
FCloudStorageStringDelegate UFGCloudStorage::OnUpdateMetadataErrorCallback;

const ANSICHAR* UFGCloudStorage::FGCloudStorageClassName = "com/ninevastudios/unrealfirebase/FGCloudStorage";

void UFGCloudStorage::UploadFromDataInMemory(const TArray<uint8>& FileInMemory, const FString& FileStoragePath,
	const FCloudStorageIntDelegate& OnUploadFromDataInMemoryProgress,
	const FCloudStorageVoidDelegate& OnSuccess,
	const FCloudStorageStringDelegate& OnError)
{
	OnUploadFromDataInMemoryProgressCallback = OnUploadFromDataInMemoryProgress;
	OnUploadFromDataInMemoryCompleteCallback = OnSuccess;
	OnUploadFromDataInMemoryFailureCallback = OnError;

	const int32 BufferSize = FileInMemory.Num();

#if PLATFORM_ANDROID
	jbyteArray jArray = FGJavaConvertor::ConvertToJByteArray(FileInMemory);

	FGMethodCallUtils::CallStaticVoidMethod(FGCloudStorageClassName, "uploadFromDataInMemory", "([BLjava/lang/String;)V", 
		jArray, *FJavaClassObject::GetJString(FileStoragePath));
#elif PLATFORM_IOS
	FIRStorage* storage = [FIRStorage storage];
	FIRStorageReference* storageRef = [storage reference];
	FIRStorageReference* storageFileRef = [storageRef child:FileStoragePath.GetNSString()];

	uint8* byteData = (uint8*)malloc(BufferSize);
	for (int32 i = 0; i < BufferSize; i++)
	{
		byteData[i] = FileInMemory[i];
	}

	NSData* data = [NSData dataWithBytes:byteData length:BufferSize];

	FIRStorageUploadTask* uploadTask = [storageFileRef putData:data metadata:nil completion:^(FIRStorageMetadata* metadata, NSError* error) {
		if (error != nil)
		{
			FString ErrorMessage = FString(error.localizedDescription);
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to upload: %s"), * ErrorMessage);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnUploadFromDataInMemoryFailureCallback.ExecuteIfBound(ErrorMessage);
			});
		}
		else
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnUploadFromDataInMemoryCompleteCallback.ExecuteIfBound();
			});
		}
	}];
	
	FIRStorageHandle observer = [uploadTask observeStatus:FIRStorageTaskStatusProgress handler:^(FIRStorageTaskSnapshot *snapshot) {
		int progress = (int) (snapshot.progress.fractionCompleted * 100);
		
		AsyncTask(ENamedThreads::GameThread, [=]() {
			UFGCloudStorage::OnUploadFromDataInMemoryProgressCallback.ExecuteIfBound(progress);
		});
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::storage::Storage* storage = firebase::storage::Storage::GetInstance(firebase::App::GetInstance());

	firebase::storage::StorageReference reference = storage->GetReference(FGUtils::GetStringCopy(FileStoragePath));

	FGStorageOperationListener* listener = new FGStorageOperationListener();
	listener->OnProgressCallback = OnUploadFromDataInMemoryProgressCallback;

	auto task = reference.PutBytes(FileInMemory.GetData(), BufferSize, listener);
	task.AddOnCompletion([=](const ::firebase::Future<firebase::storage::Metadata>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnUploadFromDataInMemoryCompleteCallback.ExecuteIfBound();
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Upload From Data In Memory error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnUploadFromDataInMemoryFailureCallback.ExecuteIfBound(ErrorMessage);
			});
		}

		FGUtils::ClearStringCopies();

		delete(listener);
	});
#endif
}

void UFGCloudStorage::UploadFromLocalFile(const FString& LocalFilePath, const FString& FileStoragePath,
	const FCloudStorageIntDelegate& OnUploadProgress,
	const FCloudStorageVoidDelegate& OnSuccess,
	const FCloudStorageStringDelegate& OnError)
{
	OnUploadFromLocalFileProgressCallback = OnUploadProgress;
	OnUploadFromLocalFileCompleteCallback = OnSuccess;
	OnUploadFromLocalFileFailureCallback = OnError;

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCloudStorageClassName, "uploadFromLocalFile", "(Ljava/lang/String;Ljava/lang/String;)V", 
		*FJavaClassObject::GetJString(LocalFilePath), *FJavaClassObject::GetJString(FileStoragePath));
#elif PLATFORM_IOS
	FIRStorage* storage = [FIRStorage storage];
	FIRStorageReference* storageRef = [storage reference];
	FIRStorageReference* storageFileRef = [storageRef child:FileStoragePath.GetNSString()];

	NSData* data = [NSData dataWithContentsOfFile:LocalFilePath.GetNSString()];

	FIRStorageUploadTask* uploadTask = [storageFileRef putData:data metadata:nil completion:^(FIRStorageMetadata* metadata, NSError* error) {
		if (error != nil)
		{
			FString ErrorMessage = FString(error.localizedDescription);
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to upload: %s"), *ErrorMessage);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnUploadFromLocalFileFailureCallback.ExecuteIfBound(ErrorMessage);
			});
		}
		else
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnUploadFromLocalFileCompleteCallback.ExecuteIfBound();
			});
		}
	}];
	
	FIRStorageHandle observer = [uploadTask observeStatus:FIRStorageTaskStatusProgress handler:^(FIRStorageTaskSnapshot *snapshot) {
		int progress = (int) (snapshot.progress.fractionCompleted * 100);
		
		AsyncTask(ENamedThreads::GameThread, [=]() {
			UFGCloudStorage::OnUploadFromLocalFileProgressCallback.ExecuteIfBound(progress);
		});
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::storage::Storage* storage = firebase::storage::Storage::GetInstance(firebase::App::GetInstance());

	firebase::storage::StorageReference reference = storage->GetReference(FGUtils::GetStringCopy(FileStoragePath));

	FGStorageOperationListener* listener = new FGStorageOperationListener();
	listener->OnProgressCallback = OnUploadFromLocalFileProgressCallback;

	auto task = reference.PutFile(FGUtils::GetStringCopy(LocalFilePath), listener);
	task.AddOnCompletion([=](const ::firebase::Future<firebase::storage::Metadata>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnUploadFromLocalFileCompleteCallback.ExecuteIfBound();
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Upload From File error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnUploadFromLocalFileFailureCallback.ExecuteIfBound(ErrorMessage);
			});
		}

		FGUtils::ClearStringCopies();

		delete(listener);
	});
#endif
}

void UFGCloudStorage::GetDownloadUrl(const FString& FileStoragePath, const FCloudStorageStringDelegate& OnSuccess,
	const FCloudStorageStringDelegate& OnError)
{
	FString downloadUrl;

	OnGetDownloadUrlCompleteCallback = OnSuccess;
	OnGetDownloadUrlFailureCallback = OnError;

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCloudStorageClassName, "getDownloadUrl", "(Ljava/lang/String;)V", *FJavaClassObject::GetJString(FileStoragePath));
#elif PLATFORM_IOS
	FIRStorage* storage = [FIRStorage storage];
	FIRStorageReference* storageRef = [storage reference];
	FIRStorageReference* storageFileRef = [storageRef child:FileStoragePath.GetNSString()];

	[storageFileRef downloadURLWithCompletion:^(NSURL* url, NSError* error) {
		if (error != nil)
		{
			FString ErrorMessage = FString(error.localizedDescription);
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to download: %s"), *ErrorMessage);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnGetDownloadUrlFailureCallback.ExecuteIfBound(ErrorMessage);
			});
		}
		else
		{
			FString result = FString(url.absoluteString);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnGetDownloadUrlCompleteCallback.ExecuteIfBound(result);
			});
		}
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::storage::Storage* storage = firebase::storage::Storage::GetInstance(firebase::App::GetInstance());

	firebase::storage::StorageReference reference = storage->GetReference(FGUtils::GetStringCopy(FileStoragePath));

	auto task = reference.GetDownloadUrl();
	task.AddOnCompletion([=](const ::firebase::Future<std::string>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			FString Url = (*callback.result()).c_str();

			UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Get Download Url: %s"), *Url);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnGetDownloadUrlCompleteCallback.ExecuteIfBound(Url);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Get Download Url error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnGetDownloadUrlFailureCallback.ExecuteIfBound(ErrorMessage);
			});
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

void UFGCloudStorage::DownloadToLocalFile(const FString& FileStoragePath, const FString& LocalFileName,
	EAndroidStorageLocation Destination,
	const FCloudStorageStringDelegate& OnSuccess,
	const FCloudStorageStringDelegate& OnError, const FCloudStorageIntDelegate& OnProgress)
{
	FString EnumAsString = UEnum::GetDisplayValueAsText(Destination).ToString();

	OnDownloadToLocalFileCompleteCallback = OnSuccess;
	OnDownloadToLocalFileErrorCallback = OnError;
	OnDownloadToLocalFileProgressCallback = OnProgress;

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCloudStorageClassName, "downloadToLocalFile", "(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
		FJavaWrapper::GameActivityThis, *FJavaClassObject::GetJString(FileStoragePath), *FJavaClassObject::GetJString(LocalFileName), *FJavaClassObject::GetJString(EnumAsString));
#elif PLATFORM_IOS
	FIRStorage* storage = [FIRStorage storage];
	FIRStorageReference* storageRef = [storage reference];
	FIRStorageReference* fileRef = [storageRef child:FileStoragePath.GetNSString()];

	NSString* documentPath = [[[NSFileManager defaultManager] URLsForDirectory:NSDocumentDirectory inDomains:NSUserDomainMask] lastObject].absoluteString;
	NSString* filePath = [documentPath stringByAppendingPathComponent:LocalFileName.GetNSString()];

	UE_LOG(LogFirebaseGoodies, Log, TEXT("Downloading file to: %s"), *FString(filePath));

	NSURL* localURL = [NSURL URLWithString:filePath];

	FIRStorageDownloadTask* downloadTask = [fileRef writeToFile:localURL completion:^(NSURL* URL, NSError* error) {
		if (error != nil)
		{
			FString ErrorMessage = FString(error.localizedDescription);
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to download: %s"), *ErrorMessage);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnDownloadToLocalFileErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}
		else
		{
			NSURL* fileUrl = (NSURL*)CFBridgingRetain(URL);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				NSString *fullPath = fileUrl.absoluteString;
				NSString *prefixToRemove = @"file:";
				if ([fullPath hasPrefix:prefixToRemove]) {
					fullPath = [fullPath substringFromIndex:[prefixToRemove length]];
				}
				UFGCloudStorage::OnDownloadToLocalFileCompleteCallback.ExecuteIfBound(FString(fullPath));
				CFBridgingRelease(fileUrl);
			});
		}
	}];
		
	FIRStorageHandle observer = [downloadTask observeStatus:FIRStorageTaskStatusProgress handler:^(FIRStorageTaskSnapshot *snapshot) {
		int progress = (int) (snapshot.progress.fractionCompleted * 100);
		
		AsyncTask(ENamedThreads::GameThread, [=]() {
			UFGCloudStorage::OnDownloadToLocalFileProgressCallback.ExecuteIfBound(progress);
		});
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::storage::Storage* storage = firebase::storage::Storage::GetInstance(firebase::App::GetInstance());

	firebase::storage::StorageReference reference = storage->GetReference(FGUtils::GetStringCopy(FileStoragePath));

	FGStorageOperationListener* listener = new FGStorageOperationListener();
	listener->OnProgressCallback = OnDownloadToLocalFileProgressCallback;

	FString FilePath = FPaths::Combine(FPaths::ProjectDir(), LocalFileName);
	FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FilePath);

	UE_LOG(LogFirebaseGoodies, Verbose, TEXT("Downloading file to: %s"), *FullPath);

	auto task = reference.GetFile(FGUtils::GetStringCopy(FullPath), listener);
	task.AddOnCompletion([=](const ::firebase::Future<size_t>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnDownloadToLocalFileCompleteCallback.ExecuteIfBound(FullPath);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Download To Local File error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnDownloadToLocalFileErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}

		FGUtils::ClearStringCopies();

		delete(listener);
	});
#endif
}

void UFGCloudStorage::DownloadInMemory(const FString& FileStoragePath, int64 FileSizeLimit, const FCloudStorageByteArrayDelegate& OnSuccess, const FCloudStorageStringDelegate& OnError)
{
	OnDownloadInMemoryCompleteCallback = OnSuccess;
	OnDownloadInMemoryErrorCallback = OnError;

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCloudStorageClassName, "downloadInMemory", "(Ljava/lang/String;J)V", 
		*FJavaClassObject::GetJString(FileStoragePath), (jlong)FileSizeLimit);
#elif PLATFORM_IOS
	FIRStorage* storage = [FIRStorage storage];
	FIRStorageReference* storageRef = [storage reference];
	FIRStorageReference* fileRef = [storageRef child:FileStoragePath.GetNSString()];

	[fileRef dataWithMaxSize:(int64)FileSizeLimit completion:^(NSData* data, NSError* error) {
		if (error != nil)
		{
			FString ErrorMessage = FString(error.localizedDescription);
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to download: %s"), *ErrorMessage);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnDownloadInMemoryErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}
		else
		{
			NSUInteger len = [data length];
			uint8* byteData = (uint8*)[data bytes];

			TArray<uint8> byteArray;
			byteArray.SetNumZeroed(len);

			for (int32 i = 0; i < len; i++)
			{
				byteArray[i] = byteData[i];
			}
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnDownloadInMemoryCompleteCallback.ExecuteIfBound(byteArray);
			});
		}
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::storage::Storage* storage = firebase::storage::Storage::GetInstance(firebase::App::GetInstance());

	firebase::storage::StorageReference reference = storage->GetReference(FGUtils::GetStringCopy(FileStoragePath));

	uint8* buffer = new uint8[FileSizeLimit];

	auto task = reference.GetBytes(buffer, FileSizeLimit);
	task.AddOnCompletion([=](const ::firebase::Future<size_t>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			size_t Size = *callback.result();
			TArray<uint8> byteArray = TArray<uint8>(buffer, FGenericPlatformMath::Min(FileSizeLimit, (int64) Size));

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnDownloadInMemoryCompleteCallback.ExecuteIfBound(byteArray);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Download In Memory error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnDownloadInMemoryErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

void UFGCloudStorage::DeleteFile(const FString& FileStoragePath, const FCloudStorageVoidDelegate& OnSuccess,
	const FCloudStorageStringDelegate& OnError)
{
	OnDeleteFileCompleteCallback = OnSuccess;
	OnDeleteFileErrorCallback = OnError;

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCloudStorageClassName, "deleteFile", "(Ljava/lang/String;)V", *FJavaClassObject::GetJString(FileStoragePath));
#elif PLATFORM_IOS
	FIRStorage* storage = [FIRStorage storage];
	FIRStorageReference* storageRef = [storage reference];
	FIRStorageReference* fileRef = [storageRef child:FileStoragePath.GetNSString()];

	[fileRef deleteWithCompletion:^(NSError* error) {
		if (error != nil)
		{
			FString ErrorMessage = FString(error.localizedDescription);
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to delete: %s"), *ErrorMessage);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnDeleteFileErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}
		else
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnDeleteFileCompleteCallback.ExecuteIfBound();
			});
		}
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::storage::Storage* storage = firebase::storage::Storage::GetInstance(firebase::App::GetInstance());

	firebase::storage::StorageReference reference = storage->GetReference(FGUtils::GetStringCopy(FileStoragePath));

	auto task = reference.Delete();
	task.AddOnCompletion([=](const ::firebase::Future<void>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnDeleteFileCompleteCallback.ExecuteIfBound();
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Delete File error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnDeleteFileErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

void UFGCloudStorage::GetFileMetadata(const FString& FileStoragePath, const FCloudStorageMetadataDelegate& OnSuccess,
	const FCloudStorageStringDelegate& OnError)
{
	OnGetMetadataCompleteCallback = OnSuccess;
	OnGetMetadataErrorCallback = OnError;

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCloudStorageClassName, "getFileMetadata", "(Ljava/lang/String;)V", *FJavaClassObject::GetJString(FileStoragePath));
#elif PLATFORM_IOS
	FIRStorage* storage = [FIRStorage storage];
	FIRStorageReference* storageRef = [storage reference];
	FIRStorageReference* fileRef = [storageRef child:FileStoragePath.GetNSString()];

	[fileRef metadataWithCompletion:^(FIRStorageMetadata* metadata, NSError* error) {
		if (error != nil)
		{
			FString ErrorMessage = FString(error.localizedDescription);
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to get metadata: %s"), *ErrorMessage);
			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnGetMetadataErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}
		else
		{
			UFGStorageMetadata* result = NewObject<UFGStorageMetadata>();
			result->Init(metadata);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnGetMetadataCompleteCallback.ExecuteIfBound(result);
			});
		}
	}];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::storage::Storage* storage = firebase::storage::Storage::GetInstance(firebase::App::GetInstance());

	firebase::storage::StorageReference reference = storage->GetReference(FGUtils::GetStringCopy(FileStoragePath));

	auto task = reference.GetMetadata();
	task.AddOnCompletion([=](const ::firebase::Future<firebase::storage::Metadata>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			UFGStorageMetadata* result = NewObject<UFGStorageMetadata>();
			result->Init(*callback.result());

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnGetMetadataCompleteCallback.ExecuteIfBound(result);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Get File Metadata error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnGetMetadataErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}

		FGUtils::ClearStringCopies();
	});
#endif
}

void UFGCloudStorage::UpdateFileMetadata(const FString& FileStoragePath, UFGStorageMetadataValues* NewMetadataValues,
	const FCloudStorageMetadataDelegate& OnSuccess,
	const FCloudStorageStringDelegate& OnError)
{
	OnUpdateMetadataCompleteCallback = OnSuccess;
	OnUpdateMetadataErrorCallback = OnError;

#if PLATFORM_ANDROID
	FGMethodCallUtils::CallStaticVoidMethod(FGCloudStorageClassName, "updateMetadata",
		"(Ljava/lang/String;Lcom/google/firebase/storage/StorageMetadata$Builder;)V",
		*FJavaClassObject::GetJString(FileStoragePath),
		NewMetadataValues->JavaMetadataBuilderObject);
#elif PLATFORM_IOS
	FIRStorage* storage = [FIRStorage storage];
	FIRStorageReference* storageRef = [storage reference];
	FIRStorageReference* fileRef = [storageRef child:FileStoragePath.GetNSString()];

	[fileRef updateMetadata:NewMetadataValues->iosMetadata
				 completion:^(FIRStorageMetadata* metadata, NSError* error) {
					 if (error != nil)
					 {
						 FString ErrorMessage = FString(error.localizedDescription);
						 UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to update metadata: %s"), *ErrorMessage);
						 AsyncTask(ENamedThreads::GameThread, [=]() {
							 UFGCloudStorage::OnUpdateMetadataErrorCallback.ExecuteIfBound(ErrorMessage);
						 });
					 }
					 else
					 {
						 UFGStorageMetadata* result = NewObject<UFGStorageMetadata>();
						 result->Init(metadata);

						 AsyncTask(ENamedThreads::GameThread, [=]() {
							 UFGCloudStorage::OnUpdateMetadataCompleteCallback.ExecuteIfBound(result);
						 });
					 }
				 }];
#elif (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::storage::Storage* storage = firebase::storage::Storage::GetInstance(firebase::App::GetInstance());

	firebase::storage::StorageReference reference = storage->GetReference(FGUtils::GetStringCopy(FileStoragePath));

	auto task = reference.UpdateMetadata(NewMetadataValues->desktopMetadata);
	task.AddOnCompletion([=](const ::firebase::Future<firebase::storage::Metadata>& callback) {
		if (callback.status() == firebase::FutureStatus::kFutureStatusComplete)
		{
			UFGStorageMetadata* result = NewObject<UFGStorageMetadata>();
			result->Init(*callback.result());

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnUpdateMetadataCompleteCallback.ExecuteIfBound(result);
			});
		}
		else
		{
			FString ErrorMessage = FString(callback.error_message());
			UE_LOG(LogFirebaseGoodies, Error, TEXT("Update File Metadata error: %s"), *ErrorMessage);

			AsyncTask(ENamedThreads::GameThread, [=]() {
				UFGCloudStorage::OnUpdateMetadataErrorCallback.ExecuteIfBound(ErrorMessage);
			});
		}
	});
#endif
}

#if PLATFORM_ANDROID

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnUploadFromDataInMemoryProgress(JNIEnv* env, jclass clazz, jint progress)
{
	int uploadProgress = FGJavaConvertor::FromJavaInt(progress);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnUploadFromLocalFileProgressCallback.ExecuteIfBound(uploadProgress);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnUploadFromDataInMemoryComplete(JNIEnv* env, jclass clazz)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnUploadFromDataInMemoryCompleteCallback.ExecuteIfBound();
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnUploadFromDataInMemoryFailure(JNIEnv* env, jclass clazz, jstring error)
{
	FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
	UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to upload: %s"), *ErrorMessage);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnUploadFromDataInMemoryFailureCallback.ExecuteIfBound(ErrorMessage);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnUploadFromLocalFileProgress(JNIEnv* env, jclass clazz, jint progress)
{
	int uploadProgress = FGJavaConvertor::FromJavaInt(progress);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnUploadFromLocalFileProgressCallback.ExecuteIfBound(uploadProgress);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnUploadFromLocalFileFailure(JNIEnv* env, jclass clazz, jstring error)
{
	FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
	UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to upload: %s"), *ErrorMessage);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnUploadFromLocalFileFailureCallback.ExecuteIfBound(ErrorMessage);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnUploadFromLocalFileComplete(JNIEnv* env, jclass clazz)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnUploadFromLocalFileCompleteCallback.ExecuteIfBound();
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnGetDownloadUrlComplete(JNIEnv* env, jclass clazz, jstring downloadUrl)
{
	FString downloadUrlString = FGJavaConvertor::FromJavaString(downloadUrl);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnGetDownloadUrlCompleteCallback.ExecuteIfBound(downloadUrlString);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnGetDownloadUrlFailureCallback(JNIEnv* env, jclass clazz, jstring error)
{
	FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
	UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to download: %s"), *ErrorMessage);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnGetDownloadUrlFailureCallback.ExecuteIfBound(ErrorMessage);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnDownloadToLocalFileComplete(JNIEnv* env, jclass clazz, jstring path)
{
	FString pathString = FGJavaConvertor::FromJavaString(path);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnDownloadToLocalFileCompleteCallback.ExecuteIfBound(pathString);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnDownloadToLocalFileError(JNIEnv* env, jclass clazz, jstring error)
{
	FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
	UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to download: %s"), *ErrorMessage);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnDownloadToLocalFileErrorCallback.ExecuteIfBound(ErrorMessage);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnDownloadToLocalFileProgress(JNIEnv* env, jclass clazz, jint progress)
{
	int uploadProgress = FGJavaConvertor::FromJavaInt(progress);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnDownloadToLocalFileProgressCallback.ExecuteIfBound(uploadProgress);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnDeleteFileComplete(JNIEnv* env, jclass clazz)
{
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnDeleteFileCompleteCallback.ExecuteIfBound();
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnDeleteFileError(JNIEnv* env, jclass clazz, jstring error)
{
	FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
	UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to delete: %s"), *ErrorMessage);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnDeleteFileErrorCallback.ExecuteIfBound(ErrorMessage);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnDownloadInMemoryComplete(JNIEnv* env, jclass clazz, jbyteArray buffer)
{
	JNIEnv* Env = FAndroidApplication::GetJavaEnv();
	TArray<uint8> byteArray = FGJavaConvertor::ConvertToByteArray(buffer);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnDownloadInMemoryCompleteCallback.ExecuteIfBound(byteArray);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnDownloadInMemoryError(JNIEnv* env, jclass clazz, jstring error)
{
	FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
	UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to download: %s"), *ErrorMessage);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnDownloadInMemoryErrorCallback.ExecuteIfBound(ErrorMessage);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnGetMetadataComplete(JNIEnv* env, jclass clazz, jobject metadata)
{
	UFGStorageMetadata* result = NewObject<UFGStorageMetadata>();
	result->Init(metadata);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnGetMetadataCompleteCallback.ExecuteIfBound(result);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnGetMetadataError(JNIEnv* env, jclass clazz, jstring error)
{
	FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
	UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to get metadata: %s"), *ErrorMessage);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnGetMetadataErrorCallback.ExecuteIfBound(ErrorMessage);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnUpdateMetadataComplete(JNIEnv* env, jclass clazz, jobject metadata)
{
	UFGStorageMetadata* result = NewObject<UFGStorageMetadata>();
	result->Init(metadata);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnUpdateMetadataCompleteCallback.ExecuteIfBound(result);
	});
}

JNI_METHOD void Java_com_ninevastudios_unrealfirebase_FGCloudStorage_OnUpdateMetadataError(JNIEnv* env, jclass clazz, jstring error)
{
	FString ErrorMessage = FGJavaConvertor::FromJavaString(error);
	UE_LOG(LogFirebaseGoodies, Error, TEXT("Failed to update metadata: %s"), *ErrorMessage);

	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudStorage::OnUpdateMetadataErrorCallback.ExecuteIfBound(ErrorMessage);
	});
}

#endif
