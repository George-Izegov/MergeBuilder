// Copyright (c) 2022 Nineva Studios

package com.ninevastudios.unrealfirebase;

import android.util.Log;

import com.google.firebase.database.ChildEventListener;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.MutableData;
import com.google.firebase.database.Query;
import com.google.firebase.database.ServerValue;
import com.google.firebase.database.Transaction;
import com.google.firebase.database.ValueEventListener;

import java.util.HashMap;

@SuppressWarnings("unused")
public class FGDatabaseRef {

	private static final int TRANSACTION_SUCCESS = 0;
	private static final int TRANSACTION_ABORT = 1;

	public static native int doTransactionCallback(long mCallbackAddr, FGMutableData data);

	public static native void onTransactionCompleteCallback(long mCallbackAddr, boolean isSuccess, String errorMessage);

	public static native void onQueryErrorCallback(String errorMessage);

	private DatabaseReference mDbRef;
	private Query mQuery = null;

	public FGDatabaseRef(String databaseUrl, String path) {
		FirebaseDatabase database;
		if (databaseUrl.isEmpty()) {
			database = FirebaseDatabase.getInstance();
		} else {
			database = FirebaseDatabase.getInstance(databaseUrl);
		}

		if (path.isEmpty()) {
			mDbRef = database.getReference();
		} else {
			mDbRef = database.getReference(path);
		}
	}

	public void SetValue(Object value, Object priority) {
		Log.d("FirebaseGoodies", "SetValue");
		if (mDbRef != null) {
			Log.d("FirebaseGoodies", "SetValue2");
			if (priority == null)
				mDbRef.setValue(value);
			else
				mDbRef.setValue(value, priority);
		}
	}

	public void SetTimestamp() {
		if (mDbRef != null) {
			mDbRef.setValue(ServerValue.TIMESTAMP);
		}
	}

	public void SetPriority(Object priority) {
		if (mDbRef != null) {
			mDbRef.setPriority(priority);
		}
	}

	public String Push() {
		if (mDbRef != null) {
			DatabaseReference newDbRef = mDbRef.push();
			return newDbRef.getKey();
		}

		return "";
	}

	public void RemoveValue() {
		if (mDbRef != null) {
			mDbRef.removeValue();
		}
	}

	public void UpdateChildren(HashMap<String, Object> update) {
		if (mDbRef != null) {
			mDbRef.updateChildren(update);
		}
	}

	private ValueEventListener mCurrentValueListener = null;
	private ChildEventListener mCurrentChildListener = null;

	public String GetKey() {
		if (mDbRef != null) {
			return mDbRef.getKey();
		}

		return "";
	}

	public void GetValue(long callbackAddr) {
		if (mDbRef != null) {
			Query q = mQuery != null ? mQuery : mDbRef;
			if (mCurrentValueListener != null)
				q.removeEventListener(mCurrentValueListener);
			q.addListenerForSingleValueEvent(new FGDatabaseValueEventListener(callbackAddr));
		}
	}

	public void AddValueListener(long callbackAddr) {
		if (mDbRef != null) {
			Query q = mQuery != null ? mQuery : mDbRef;
			if (mCurrentValueListener != null)
				q.removeEventListener(mCurrentValueListener);
			mCurrentValueListener = q.addValueEventListener(new FGDatabaseValueEventListener(callbackAddr));
		}
	}

	public void RemoveValueListener() {
		if (mDbRef != null && mCurrentValueListener != null) {
			Query q = mQuery != null ? mQuery : mDbRef;
			q.removeEventListener(mCurrentValueListener);
			mCurrentValueListener = null;
		}
	}

	public void AddChildListener(long callbackAddr) {
		if (mDbRef != null) {
			Query q = mQuery != null ? mQuery : mDbRef;
			if (mCurrentChildListener != null)
				q.removeEventListener(mCurrentChildListener);
			mCurrentChildListener = q.addChildEventListener(new FGDatabaseChildEventListener(callbackAddr));
		}
	}

	public void RemoveChildListener() {
		if (mDbRef != null && mCurrentChildListener != null) {
			Query q = mQuery != null ? mQuery : mDbRef;
			q.removeEventListener(mCurrentChildListener);
			mCurrentChildListener = null;
		}
	}

	public void KeepSynced(boolean sync) {
		if (mDbRef != null) {
			mDbRef.keepSynced(sync);
		}
	}

	public void OrderByChild(String path) {
		if (mDbRef == null) return;

		try {
			Query q = mQuery != null ? mQuery : mDbRef;
			mQuery = q.orderByChild(path);
		} catch (Exception e) {
			onQueryErrorCallback(e.getMessage());
		}
	}

	public void OrderByKey() {
		if (mDbRef == null) return;

		try {
			Query q = mQuery != null ? mQuery : mDbRef;
			mQuery = q.orderByKey();
		} catch (Exception e) {
			onQueryErrorCallback(e.getMessage());
		}
	}

	public void OrderByPriority() {
		if (mDbRef == null) return;

		try {
			Query q = mQuery != null ? mQuery : mDbRef;
			mQuery = q.orderByPriority();
		} catch (Exception e) {
			onQueryErrorCallback(e.getMessage());
		}
	}

	public void OrderByValue() {
		if (mDbRef == null) return;

		try {
			Query q = mQuery != null ? mQuery : mDbRef;
			mQuery = q.orderByValue();
		} catch (Exception e) {
			onQueryErrorCallback(e.getMessage());
		}
	}

	public void StartAtFloat(double value, String key) {
		if (mDbRef == null) return;

		try {
			Query q = mQuery != null ? mQuery : mDbRef;
			if (key.isEmpty()) {
				mQuery = q.startAt(value);
			} else {
				mQuery = q.startAt(value, key);
			}
		} catch (Exception e) {
			onQueryErrorCallback(e.getMessage());
		}
	}

	public void StartAtString(String value, String key) {
		if (mDbRef == null) return;

		try {
			Query q = mQuery != null ? mQuery : mDbRef;
			if (key.isEmpty()) {
				mQuery = q.startAt(value);
			} else {
				mQuery = q.startAt(value, key);
			}
		} catch (Exception e) {
			onQueryErrorCallback(e.getMessage());
		}
	}

	public void EndAtFloat(double value, String key) {
		if (mDbRef == null) return;

		try {
			Query q = mQuery != null ? mQuery : mDbRef;
			if (key.isEmpty()) {
				mQuery = q.endAt(value);
			} else {
				mQuery = q.endAt(value, key);
			}
		} catch (Exception e) {
			onQueryErrorCallback(e.getMessage());
		}
	}

	public void EndAtString(String value, String key) {
		if (mDbRef == null) return;

		try {
			Query q = mQuery != null ? mQuery : mDbRef;
			if (key.isEmpty()) {
				mQuery = q.endAt(value);
			} else {
				mQuery = q.endAt(value, key);
			}
		} catch (Exception e) {
			onQueryErrorCallback(e.getMessage());
		}
	}

	public void EqualToFloat(double value, String key) {
		if (mDbRef == null) return;

		try {
			Query q = mQuery != null ? mQuery : mDbRef;
			if (key.isEmpty()) {
				mQuery = q.equalTo(value);
			} else {
				mQuery = q.equalTo(value, key);
			}
		} catch (Exception e) {
			onQueryErrorCallback(e.getMessage());
		}
	}

	public void EqualToBool(boolean value, String key) {
		if (mDbRef == null) return;

		try {
			Query q = mQuery != null ? mQuery : mDbRef;
			if (key.isEmpty()) {
				mQuery = q.equalTo(value);
			} else {
				mQuery = q.equalTo(value, key);
			}
		} catch (Exception e) {
			onQueryErrorCallback(e.getMessage());
		}
	}

	public void EqualToString(String value, String key) {
		if (mDbRef == null) return;

		try {
			Query q = mQuery != null ? mQuery : mDbRef;
			if (key.isEmpty()) {
				mQuery = q.equalTo(value);
			} else {
				mQuery = q.equalTo(value, key);
			}
		} catch (Exception e) {
			onQueryErrorCallback(e.getMessage());
		}
	}

	public void LimitToFirst(int limit) {
		if (mDbRef == null) return;

		try {
			Query q = mQuery != null ? mQuery : mDbRef;
			mQuery = q.limitToFirst(limit);
		} catch (Exception e) {
			onQueryErrorCallback(e.getMessage());
		}
	}

	public void LimitToLast(int limit) {
		if (mDbRef == null) return;

		try {
			Query q = mQuery != null ? mQuery : mDbRef;
			mQuery = q.limitToLast(limit);
		} catch (Exception e) {
			onQueryErrorCallback(e.getMessage());
		}
	}

	public void OnDisconnectSetValue(Object value) {
		if (mDbRef != null) {
			mDbRef.onDisconnect().setValue(value);
		}
	}

	public void OnDisconnectSetTimestamp() {
		if (mDbRef != null) {
			mDbRef.onDisconnect().setValue(ServerValue.TIMESTAMP);
		}
	}

	public void OnDisconnectRemoveValue() {
		if (mDbRef != null) {
			mDbRef.onDisconnect().removeValue();
		}
	}

	public void OnDisconnectUpdateChildren(HashMap<String, Object> update) {
		if (mDbRef != null) {
			mDbRef.onDisconnect().updateChildren(update);
		}
	}

	public void CancelDisconnectOperations() {
		if (mDbRef != null) {
			mDbRef.onDisconnect().cancel();
		}
	}

	public void RunTransaction(final long transactionObj) {
		if (mDbRef != null) {
			mDbRef.runTransaction(new Transaction.Handler() {
				@Override
				public Transaction.Result doTransaction(MutableData mutableData) {
					// this function is called multiple times and sometimes the data in null
					// if it is return success, it will be called with proper data later
					if (mutableData.getValue() == null) return Transaction.success(mutableData);

					FGMutableData data = new FGMutableData(mutableData);
					int result = doTransactionCallback(transactionObj, data);
					return result == TRANSACTION_SUCCESS ? Transaction.success(mutableData) : Transaction.abort();
				}

				@Override
				public void onComplete(DatabaseError databaseError, boolean b, DataSnapshot dataSnapshot) {
					String errorMessage = databaseError != null ? databaseError.getMessage() : "";
					onTransactionCompleteCallback(transactionObj, b, errorMessage);
				}
			});
		}
	}
}
