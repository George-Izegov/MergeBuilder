// Copyright (c) 2022 Nineva Studios

package com.ninevastudios.unrealfirebase;

import com.google.firebase.database.DataSnapshot;

import java.util.ArrayList;

public class FGDataSnapshot {

	private DataSnapshot mDataSnapshot = null;

	public FGDataSnapshot(DataSnapshot dataSnapshot) {
		mDataSnapshot = dataSnapshot;
	}

	public String GetKey() {
		return mDataSnapshot.getKey();
	}

	public Object GetPriority() {
		return mDataSnapshot.getPriority();
	}

	public Object GetValue() {
		return mDataSnapshot.getValue();
	}

	public boolean Exists() {
		return mDataSnapshot.exists();
	}

	public Object Child(String path) {
		FGDataSnapshot data = new FGDataSnapshot(mDataSnapshot.child(path));
		return data;
	}

	public ArrayList<Object> GetChildren() {
		ArrayList<Object> dataArray = new ArrayList<>();
		for (DataSnapshot child : mDataSnapshot.getChildren()) {
			dataArray.add(new FGDataSnapshot(child));
		}

		return dataArray;
	}

	public long GetChildrenCount() {
		return mDataSnapshot.getChildrenCount();
	}

	public boolean HasChild(String path) {
		return mDataSnapshot.hasChild(path);
	}

	public boolean HasChildren() {
		return mDataSnapshot.hasChildren();
	}
}
