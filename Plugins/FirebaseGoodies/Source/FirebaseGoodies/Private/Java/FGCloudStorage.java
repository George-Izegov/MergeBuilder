// Copyright (c) 2022 Nineva Studios

package com.ninevastudios.unrealfirebase;

import android.app.Activity;
import android.net.Uri;

import androidx.annotation.NonNull;

import com.google.android.gms.tasks.OnFailureListener;
import com.google.android.gms.tasks.OnSuccessListener;
import com.google.firebase.storage.FileDownloadTask;
import com.google.firebase.storage.FirebaseStorage;
import com.google.firebase.storage.OnProgressListener;
import com.google.firebase.storage.StorageMetadata;
import com.google.firebase.storage.StorageReference;
import com.google.firebase.storage.UploadTask;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;

@SuppressWarnings("unused")
public class FGCloudStorage {
	private static final FirebaseStorage storage = FirebaseStorage.getInstance();
	private static final StorageReference storageRef = storage.getReference();

	public static native void OnUploadFromLocalFileProgress(int progress);
	public static native void OnUploadFromLocalFileFailure(String error);
	public static native void OnUploadFromLocalFileComplete();

	public static native void OnUploadFromDataInMemoryProgress(int progress);
	public static native void OnUploadFromDataInMemoryFailure(String error);
	public static native void OnUploadFromDataInMemoryComplete();

	public static native void OnDownloadInMemoryComplete(byte[] bytes);
	public static native void OnDownloadInMemoryError(String error);

	public static native void OnDownloadToLocalFileProgress(int progress);
	public static native void OnDownloadToLocalFileComplete(String path);
	public static native void OnDownloadToLocalFileError(String error);

	public static native void OnGetMetadataComplete(StorageMetadata storageMetadata);
	public static native void OnGetMetadataError(String error);

	public static native void OnUpdateMetadataComplete(StorageMetadata storageMetadata);
	public static native void OnUpdateMetadataError(String error);

	public static native void OnDeleteFileComplete();
	public static native void OnDeleteFileError(String error);

	public static native void OnGetDownloadUrlComplete(String downloadUrl);
	public static native void OnGetDownloadUrlError(String error);

	public static void uploadFromDataInMemory(byte[] buffer , String storagePath) {
		StorageReference uploadedRef = storageRef.child(storagePath);

		UploadTask uploadTask = uploadedRef.putBytes(buffer);
		uploadTask.addOnProgressListener(new OnProgressListener<UploadTask.TaskSnapshot>() {
			@Override
			public void onProgress(UploadTask.TaskSnapshot taskSnapshot) {
				float progress = (100.0f * taskSnapshot.getBytesTransferred()) / taskSnapshot.getTotalByteCount();
				OnUploadFromDataInMemoryProgress((int)progress);
			}
		}).addOnFailureListener(new OnFailureListener() {
			@Override
			public void onFailure(Exception e) {
				OnUploadFromDataInMemoryFailure(e.toString());
			}
		}).addOnSuccessListener(new OnSuccessListener<UploadTask.TaskSnapshot>() {
			@Override
			public void onSuccess(UploadTask.TaskSnapshot taskSnapshot) {
				OnUploadFromDataInMemoryComplete();
			}
		});
	}

	public static void uploadFromLocalFile(String filePath, final String storagePath) {
		final Uri file = Uri.fromFile(new File(filePath));

		final UploadTask uploadTask = storageRef
				.child(storagePath).putFile(file);

		uploadTask.addOnProgressListener(new OnProgressListener<UploadTask.TaskSnapshot>() {
			@Override
			public void onProgress(UploadTask.TaskSnapshot taskSnapshot) {
				float progress = (100.0f * taskSnapshot.getBytesTransferred()) / taskSnapshot.getTotalByteCount();

				OnUploadFromLocalFileProgress((int)progress);
			}
		}).addOnFailureListener(new OnFailureListener() {
			@Override
			public void onFailure(Exception e) {
				OnUploadFromLocalFileFailure(e.toString());
			}
		}).addOnSuccessListener(new OnSuccessListener<UploadTask.TaskSnapshot>() {
			@Override
			public void onSuccess(UploadTask.TaskSnapshot taskSnapshot) {
				OnUploadFromLocalFileComplete();
			}
		});
	}

	public static void getDownloadUrl(String fileStoragePath)
	{
		storageRef.child(fileStoragePath).getDownloadUrl()
				.addOnSuccessListener(new OnSuccessListener<Uri>() {
			@Override
			public void onSuccess(Uri uri) {
				OnGetDownloadUrlComplete(uri.toString());
			}
		}).addOnFailureListener(new OnFailureListener() {
			@Override
			public void onFailure(Exception e) {
				OnGetDownloadUrlError(e.toString());
			}
		});
	}

	public static void downloadInMemory(String storageFilePath, long fileSizeLimit) {
		StorageReference fileRef = storageRef.child(storageFilePath);

		fileRef.getBytes(fileSizeLimit).addOnSuccessListener(new OnSuccessListener<byte[]>() {
			@Override
			public void onSuccess(byte[] bytes) {
				OnDownloadInMemoryComplete(bytes);
			}
		}).addOnFailureListener(new OnFailureListener() {
			@Override
			public void onFailure(Exception e) {
				OnDownloadInMemoryError(e.toString());
			}
		});
	}

	public static void downloadToLocalFile(Activity context, String storageFilePath, final String localFileName, String environment) throws IOException {
		StorageReference fileRef = storageRef.child(storageFilePath);

		File root = context.getExternalFilesDir(environment);
		final File localFile = new File(root, localFileName);

		fileRef.getFile(localFile).addOnSuccessListener(new OnSuccessListener<FileDownloadTask.TaskSnapshot>() {
			@Override
			public void onSuccess(FileDownloadTask.TaskSnapshot taskSnapshot) {
				OnDownloadToLocalFileComplete(localFile.getAbsolutePath());
			}
		}).addOnFailureListener(new OnFailureListener() {
			@Override
			public void onFailure(Exception e) {
				OnDownloadToLocalFileError(e.toString());
			}
		}).addOnProgressListener(new OnProgressListener<FileDownloadTask.TaskSnapshot>() {
			@Override
			public void onProgress(@NonNull FileDownloadTask.TaskSnapshot snapshot) {
				float progress = (100.0f * snapshot.getBytesTransferred()) / snapshot.getTotalByteCount();
				OnDownloadToLocalFileProgress((int) progress);
			}
		});
	}

	public static void getFileMetadata(String storageFilePath) {
		StorageReference fileRef = storageRef.child(storageFilePath);

		fileRef.getMetadata().addOnSuccessListener(new OnSuccessListener<StorageMetadata>() {
			@Override
			public void onSuccess(StorageMetadata storageMetadata) {
				OnGetMetadataComplete(storageMetadata);
			}
		}).addOnFailureListener(new OnFailureListener() {
			@Override
			public void onFailure(Exception e) {
				OnGetMetadataError(e.toString());
			}
		});
	}

	public static void updateMetadata(String storageFilePath, StorageMetadata.Builder builder) {
		StorageReference fileRef = storageRef.child(storageFilePath);

		StorageMetadata metadata = builder.build();
		fileRef.updateMetadata(metadata).addOnSuccessListener(new OnSuccessListener<StorageMetadata>() {
			@Override
			public void onSuccess(StorageMetadata storageMetadata) {
				OnUpdateMetadataComplete(storageMetadata);
			}
		}).addOnFailureListener(new OnFailureListener() {
			@Override
				public void onFailure(Exception e) {
				OnUpdateMetadataError(e.toString());
			}
		});
	}

	public static void deleteFile(final String storageFilePath) {
		StorageReference fileRef = storageRef.child(storageFilePath);

		fileRef.delete().addOnSuccessListener(new OnSuccessListener<Void>() {
			@Override
			public void onSuccess(Void aVoid) {
				OnDeleteFileComplete();
			}
		}).addOnFailureListener(new OnFailureListener() {
			@Override
			public void onFailure(Exception e) {
				OnDeleteFileError(e.toString());
			}
		});
	}

	public static HashMap<String, String> getCustomMetadata(StorageMetadata metadata) {
		HashMap<String, String> result = new HashMap<>();

		for (String key: metadata.getCustomMetadataKeys()) {
			result.put(key, metadata.getCustomMetadata(key));
		}

		return result;
	}

	public static StorageMetadata.Builder getMetadataBuilder() {
		return new StorageMetadata.Builder();
	}
}