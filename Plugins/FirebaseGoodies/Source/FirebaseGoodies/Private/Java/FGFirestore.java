package com.ninevastudios.unrealfirebase;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.firestore.CollectionReference;
import com.google.firebase.firestore.DocumentReference;
import com.google.firebase.firestore.DocumentSnapshot;
import com.google.firebase.firestore.EventListener;
import com.google.firebase.firestore.FieldValue;
import com.google.firebase.firestore.FirebaseFirestore;
import com.google.firebase.firestore.FirebaseFirestoreException;
import com.google.firebase.firestore.ListenerRegistration;
import com.google.firebase.firestore.MetadataChanges;
import com.google.firebase.firestore.Query;
import com.google.firebase.firestore.QuerySnapshot;
import com.google.firebase.firestore.SetOptions;
import com.google.firebase.firestore.Source;
import com.google.firebase.firestore.WriteBatch;
import com.google.firebase.firestore.util.Executors;

import java.util.Map;
import java.util.concurrent.Executor;

@SuppressWarnings("unused")
public class FGFirestore {

	public static native void OnItemAddSuccess(long callback, DocumentReference documentReference);
	public static native void OnItemAddError(long callback, String error);

	public static native void OnSetDocumentDataSuccess(long callback);
	public static native void OnSetDocumentDataError(long callback, String error);

	public static native void OnUpdateDocumentDataSuccess(long callback);
	public static native void OnUpdateDocumentDataError(long callback, String error);

	public static native void OnDeleteDocumentDataSuccess(long callback);
	public static native void OnDeleteDocumentDataError(long callback, String error);

	public static native void OnGetDocumentSnapshotSuccess(long callback, DocumentSnapshot data);
	public static native void OnGetDocumentSnapshotError(long callback, String error);

	public static native void OnCommitBatchSuccess(long callback);
	public static native void OnCommitBatchError(long callback, String error);

	public static native void OnGetQuerySnapshotSuccess(long callback, QuerySnapshot snapshot);
	public static native void OnGetQuerySnapshotError(long callback, String error);

	public static native void OnDocumentReferenceSnapshotEvent(DocumentSnapshot data, long cppReferencePointer);
	public static native void OnDocumentReferenceSnapshotEventError(int code, long cppReferencePointer);

	public static native void OnQuerySnapshotEvent(QuerySnapshot data, long cppReferencePointer);
	public static native void OnQuerySnapshotEventError(int code, long cppReferencePointer);

	public static CollectionReference getCollection(String path) {
		return FirebaseFirestore.getInstance().collection(path);
	}

	public static DocumentReference getDocument(String path) {
		return FirebaseFirestore.getInstance().document(path);
	}

	public static void addItem(CollectionReference collection, Object data, final long onSuccess, final long onError) {
		if (collection == null) {
			OnItemAddError(onSuccess, "Collection is not valid.");
			return;
		}

		collection.add(data).addOnCompleteListener(new OnCompleteListener<DocumentReference>() {
			@Override
			public void onComplete(@NonNull Task<DocumentReference> task) {
				if (task.isSuccessful()) {
					OnItemAddSuccess(onSuccess, task.getResult());
					return;
				}

				if (task.getException() != null) {
					OnItemAddError(onError, task.getException().toString());
				}
			}
		});
	}

	public static WriteBatch getBatch() {
		return FirebaseFirestore.getInstance().batch();
	}

	private static Source parseSource(int sourceInt) {
		switch (sourceInt) {
			case 1:
				return Source.SERVER;
			case 2:
				return Source.CACHE;
			default:
				return Source.DEFAULT;
		}
	}

	public static void setDocumentData(DocumentReference documentReference, Object data, int options, final long onSuccess, final long onError) {
		Task<Void> task;
		if (options == 1) {
			SetOptions setOptions = SetOptions.merge();
			task = documentReference.set(data, setOptions);
		} else {
			task = documentReference.set(data);
		}

		task.addOnCompleteListener(new OnCompleteListener<Void>() {
			@Override
			public void onComplete(@NonNull Task<Void> task) {
				if (task.isSuccessful()) {
					OnSetDocumentDataSuccess(onSuccess);
					return;
				}

				if (task.getException() != null) {
					OnSetDocumentDataError(onError, task.getException().toString());
				}
			}
		});
	}

	public static void updateDocumentData(DocumentReference documentReference, Map<String, Object> data, final long onSuccess, final long onError) {
		documentReference.update(data).addOnCompleteListener(new OnCompleteListener<Void>() {
			@Override
			public void onComplete(@NonNull Task<Void> task) {
				if (task.isSuccessful()) {
					OnUpdateDocumentDataSuccess(onSuccess);
					return;
				}

				if (task.getException() != null) {
					OnUpdateDocumentDataError(onError, task.getException().toString());
				}
			}
		});
	}

	public static void deleteDocument(DocumentReference documentReference, final long onSuccess, final long onError) {
		documentReference.delete().addOnCompleteListener(new OnCompleteListener<Void>() {
			@Override
			public void onComplete(@NonNull Task<Void> task) {
				if (task.isSuccessful()) {
					OnDeleteDocumentDataSuccess(onSuccess);
					return;
				}

				if (task.getException() != null) {
					OnDeleteDocumentDataError(onError, task.getException().toString());
				}
			}
		});
	}

	public static void getDocumentSnapshot(DocumentReference documentReference, int source, final long onSuccess, final long onError) {
		Source documentSource = parseSource(source);

		documentReference.get(documentSource).addOnCompleteListener(new OnCompleteListener<DocumentSnapshot>() {
			@Override
			public void onComplete(@NonNull Task<DocumentSnapshot> task) {
				if (task.isSuccessful()) {
					OnGetDocumentSnapshotSuccess(onSuccess, task.getResult());
					return;
				}

				if (task.getException() != null) {
					OnGetDocumentSnapshotError(onError, task.getException().toString());
				}
			}
		});
	}

	public static void commitBatch(WriteBatch batch, final long onSuccess, final long onError) {
		batch.commit().addOnCompleteListener(new OnCompleteListener<Void>() {
			@Override
			public void onComplete(@NonNull Task<Void> task) {
				if (task.isSuccessful()) {
					OnCommitBatchSuccess(onSuccess);
					return;
				}

				if (task.getException() != null) {
					OnCommitBatchError(onError, task.getException().toString());
				}
			}
		});
	}

	public static Query getCollectionQuery(String collectionId) {
		return FirebaseFirestore.getInstance().collectionGroup(collectionId);
	}

	public static void getQuerySnapshot(Query query, int source, final long onSuccess, final long onError) {
		Source querySource = parseSource(source);
		query.get(querySource).addOnCompleteListener(new OnCompleteListener<QuerySnapshot>() {
			@Override
			public void onComplete(@NonNull Task<QuerySnapshot> task) {
				if (task.isSuccessful()) {
					OnGetQuerySnapshotSuccess(onSuccess, task.getResult());
					return;
				}

				if (task.getException() != null) {
					OnGetQuerySnapshotError(onError, task.getException().toString());
				}
			}
		});
	}

	public static Query orderQueryBy(Query query, String field, boolean ascending) {
		return query.orderBy(field, ascending ? Query.Direction.ASCENDING : Query.Direction.DESCENDING);
	}

	public static DocumentSnapshot[] getQuerySnapshotDocuments(QuerySnapshot snapshot) {
		return snapshot.getDocuments().toArray(new DocumentSnapshot[0]);
	}

	public static boolean hasDocumentSnapshotPendingWrites(DocumentSnapshot snapshot) {
		return snapshot.getMetadata().hasPendingWrites();
	}

	public static boolean isDocumentSnapshotFromCache(DocumentSnapshot snapshot) {
		return snapshot.getMetadata().isFromCache();
	}

	public static boolean hasQuerySnapshotPendingWrites(QuerySnapshot snapshot) {
		return snapshot.getMetadata().hasPendingWrites();
	}

	public static boolean isQuerySnapshotFromCache(QuerySnapshot snapshot) {
		return snapshot.getMetadata().isFromCache();
	}

	public static ListenerRegistration addDocumentReferenceListener(DocumentReference reference, int executorType, boolean includeOnlyMetadataChanges, final long cppReferencePointer) {
		Executor executor;
		if (executorType == 1) {
			executor = Executors.DIRECT_EXECUTOR;
		} else if (executorType == 2) {
			executor = Executors.BACKGROUND_EXECUTOR;
		} else {
			executor = Executors.DEFAULT_CALLBACK_EXECUTOR;
		}

		return reference.addSnapshotListener(
				executor,
				includeOnlyMetadataChanges ? MetadataChanges.INCLUDE : MetadataChanges.EXCLUDE,
				new EventListener<DocumentSnapshot>() {
			@Override
			public void onEvent(@Nullable DocumentSnapshot value, @Nullable FirebaseFirestoreException error) {
				if (value != null) {
					OnDocumentReferenceSnapshotEvent(value, cppReferencePointer);
					return;
				}

				if (error != null) {
					OnDocumentReferenceSnapshotEventError(error.getCode().value(), cppReferencePointer);
				}
			}
		});
	}

	public static ListenerRegistration addQueryListener(Query query, int executorType, boolean includeOnlyMetadataChanges, final long cppReferencePointer) {
		Executor executor;
		if (executorType == 1) {
			executor = Executors.DIRECT_EXECUTOR;
		} else if (executorType == 2) {
			executor = Executors.BACKGROUND_EXECUTOR;
		} else {
			executor = Executors.DEFAULT_CALLBACK_EXECUTOR;
		}

		return query.addSnapshotListener(
				executor,
				includeOnlyMetadataChanges ? MetadataChanges.INCLUDE : MetadataChanges.EXCLUDE,
				new EventListener<QuerySnapshot>() {
					@Override
					public void onEvent(@Nullable QuerySnapshot value, @Nullable FirebaseFirestoreException error) {
						if (value != null) {
							OnQuerySnapshotEvent(value, cppReferencePointer);
							return;
						}

						if (error != null) {
							OnQuerySnapshotEventError(error.getCode().value(), cppReferencePointer);
						}
					}
				});
	}

	public static FieldValue serverTimestamp() {
		return FieldValue.serverTimestamp();
	}
}
