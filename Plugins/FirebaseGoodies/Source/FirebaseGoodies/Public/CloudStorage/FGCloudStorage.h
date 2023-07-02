// Copyright (c) 2022 Nineva Studios

#pragma once

#include "FGStorageMetadata.h"
#include "FGStorageMetadataValues.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "FGCloudStorage.generated.h"

UENUM(BlueprintType)
enum class EAndroidStorageLocation : uint8
{
	Music = 0,
	Podcasts = 1,
	Ringtones = 2,
	Alarms = 3,
	Notifications = 4,
	Pictures = 5,
	Movies = 6,
	Downloads = 7,
	DCIM = 8,
	Documents = 9,
	Audiobooks = 10
};

DECLARE_DYNAMIC_DELEGATE(FCloudStorageVoidDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCloudStorageIntDelegate, int, intNum);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCloudStorageStringDelegate, FString, string);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCloudStorageByteArrayDelegate, const TArray<uint8>&, byteArray);
DECLARE_DYNAMIC_DELEGATE_OneParam(FCloudStorageMetadataDelegate, UFGStorageMetadata*, metadata);

UCLASS()
class FIREBASEGOODIES_API UFGCloudStorage : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	*  __________________________________________________________________
	* |																	 |
	* | Prior to using firebase cloud storage authorisation is required. |
	* |__________________________________________________________________|
	*/

	/**
	* Upload file from memory buffer.
	*
	* @param FileInMemory - file to upload from data in memory.
	* @param FileStoragePath - file's cloud storage path, full file name must be included, e.g. "Pictures/Carl.png"
	* @param OnUploadFromDataInMemoryProgress - callback to be invoked upon uploading progress.
	* @param OnSuccess - callback to be invoked upon successful file upload.
	* @param OnError - callback to be invoked when upload failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage", meta = (AutocreateRefTerm = "OnUploadFromDataInMemoryProgress, OnSuccess, OnError"))
	static void UploadFromDataInMemory(const TArray<uint8>& FileInMemory, const FString& FileStoragePath,
		const FCloudStorageIntDelegate& OnUploadFromDataInMemoryProgress,
		const FCloudStorageVoidDelegate& OnSuccess, const FCloudStorageStringDelegate& OnError);

	/**
	* Upload file from device.
	* 
	* @param LocalFilePath - file's full path on device.
	* @param FileStoragePath - file's cloud storage path, full file name must be included, e.g. "Pictures/Carl.png"
	* @param OnUploadProgress - callback to be invoked upon uploading progress. Only for Android.
	* @param OnSuccess - callback to be invoked upon successful file upload.
	* @param OnError - callback to be invoked when upload failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage", meta = (AutoCreateRefTerm = "OnUploadProgress, OnSuccess, OnError"))
	static void UploadFromLocalFile(const FString& LocalFilePath, const FString& FileStoragePath,
		const FCloudStorageIntDelegate& OnUploadProgress,
		const FCloudStorageVoidDelegate& OnSuccess, const FCloudStorageStringDelegate& OnError);

	/**
	* Receive download Url.
	*
	* @param FileStoragePath - file's cloud storage path, full file name must be included, e.g. "Pictures/Carl.png"
	* @param OnSuccess - callback to be invoked upon successful download Url retrieval.
	* @param OnError - callback to be invoked when retrieving download Url failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage", meta = (AutoCreateRefTerm = "OnSuccessUrl, OnError"))
	static void GetDownloadUrl(const FString& FileStoragePath, const FCloudStorageStringDelegate& OnSuccess,
		const FCloudStorageStringDelegate& OnError);

	/**
	* Download file to device.
	*
	* @param FileStoragePath - file's cloud storage path, full file name must be included, e.g. "Pictures/Carl.png"
	* @param LocalFileName - set name of the downloaded file, type of file must be included in file name, e.g. "Carl.png"
	* @param Destination - environment directory for the file to be downloaded to. Android only.
	* @param OnSuccess - callback to be invoked upon successful download to device. Returns a full path to the downloaded file.
	* @param OnError - callback to be invoked when download to device failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage", meta = (AutoCreateRefTerm = "OnSuccess, OnError, OnProgress"))
	static void DownloadToLocalFile(const FString& FileStoragePath, const FString& LocalFileName,
		EAndroidStorageLocation Destination,
		const FCloudStorageStringDelegate& OnSuccess, const FCloudStorageStringDelegate& OnError, const FCloudStorageIntDelegate& OnProgress);

	/**
	* Delete file from cloud storage.
	*
	* @param FileStoragePath - file's cloud storage path, full file name must be included, e.g. "Pictures/Carl.png"
	* @param OnSuccess - callback to be invoked upon successful file removal.
	* @param OnError - callback to be invoked when removal from cloud storage failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage", meta = (AutoCreateRefTerm = "OnSuccess, OnError"))
	static void DeleteFile(const FString& FileStoragePath, const FCloudStorageVoidDelegate& OnSuccess,
		const FCloudStorageStringDelegate& OnError);

	/**
	* Download file in memory buffer.
	*
	* @param FileStoragePath - file's cloud storage path, full file name must be included, e.g. "Pictures/Carl.png"
	* @param FileSizeLimit - limitation of memory buffer in Bytes.
	* @param OnSuccess - callback to be invoked upon successful download of file to memory buffer.
	* @param OnError - callback to be invoked when downloading to memory buffer failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage", meta = (AutoCreateRefTerm = "OnSuccess, OnError, OnProgress"))
	static void DownloadInMemory(const FString& FileStoragePath, int64 FileSizeLimit,
		const FCloudStorageByteArrayDelegate& OnSuccess, const FCloudStorageStringDelegate& OnError);

	/**
	* Receive file metadata.
	*
	* @param FileStoragePath - file's cloud storage path, full file name must be included, e.g. "Pictures/Carl.png"
	* @param OnSuccess - callback to be invoked upon successful metadata retrieval. Object of class FGStorageMetadata will be received.
	* @param OnError - callback to be invoked when retrieving metadata failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage", meta = (AutoCreateRefTerm = "OnSuccess, OnError"))
	static void GetFileMetadata(const FString& FileStoragePath, const FCloudStorageMetadataDelegate& OnSuccess,
		const FCloudStorageStringDelegate& OnError);

	/**
	* Update file metadata.
	* Creation of NewStorageMetadataValues object is required for setting new metadata values.
	*
	* @param FileStoragePath - file's cloud storage path, full file name must be included, e.g. "Pictures/Carl.png"
	* @param NewMetadataValues - metadata values to be set for chosen file. 
	* @param OnSuccess - callback to be invoked upon successful metadata update. 
	* @param OnError - callback to be invoked when metadata update failed.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase Goodies|Cloud Storage", meta = (AutoCreateRefTerm = "OnSuccess, OnError"))
	static void UpdateFileMetadata(const FString& FileStoragePath, UFGStorageMetadataValues* NewMetadataValues,
		const FCloudStorageMetadataDelegate& OnSuccess, const FCloudStorageStringDelegate& OnError);

	static FCloudStorageIntDelegate OnUploadFromDataInMemoryProgressCallback;
	static FCloudStorageVoidDelegate OnUploadFromDataInMemoryCompleteCallback;
	static FCloudStorageStringDelegate OnUploadFromDataInMemoryFailureCallback;

	static FCloudStorageIntDelegate OnUploadFromLocalFileProgressCallback;
	static FCloudStorageVoidDelegate OnUploadFromLocalFileCompleteCallback;
	static FCloudStorageStringDelegate OnUploadFromLocalFileFailureCallback;

	static FCloudStorageStringDelegate OnGetDownloadUrlCompleteCallback;
	static FCloudStorageStringDelegate OnGetDownloadUrlFailureCallback;

	static FCloudStorageStringDelegate OnDownloadToLocalFileCompleteCallback;
	static FCloudStorageStringDelegate OnDownloadToLocalFileErrorCallback;
	static FCloudStorageIntDelegate OnDownloadToLocalFileProgressCallback;

	static FCloudStorageVoidDelegate OnDeleteFileCompleteCallback;
	static FCloudStorageStringDelegate OnDeleteFileErrorCallback;

	static FCloudStorageByteArrayDelegate OnDownloadInMemoryCompleteCallback;
	static FCloudStorageStringDelegate OnDownloadInMemoryErrorCallback;
	static FCloudStorageIntDelegate OnDownloadInMemoryProgressCallback;

	static FCloudStorageMetadataDelegate OnGetMetadataCompleteCallback;
	static FCloudStorageStringDelegate OnGetMetadataErrorCallback;

	static FCloudStorageMetadataDelegate OnUpdateMetadataCompleteCallback;
	static FCloudStorageStringDelegate OnUpdateMetadataErrorCallback;

	static const ANSICHAR* FGCloudStorageClassName;
};
