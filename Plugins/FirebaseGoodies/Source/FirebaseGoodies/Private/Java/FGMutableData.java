// Copyright (c) 2022 Nineva Studios

package com.ninevastudios.unrealfirebase;

import com.google.firebase.database.MutableData;

import java.util.ArrayList;

@SuppressWarnings("unused")
public class FGMutableData {

	private MutableData mMutableData = null;

	public FGMutableData(MutableData mutableData) {
		mMutableData = mutableData;
	}

	public String GetKey() {
		return mMutableData.getKey();
	}

	public Object GetPriority() {
		return mMutableData.getPriority();
	}

	public void SetPriority(Object priority) {
		mMutableData.setPriority(priority);
	}

	public Object GetValue() {
		return mMutableData.getValue();
	}

	public void SetValue(Object value) {
		mMutableData.setValue(value);
	}

	public Object Child(String path) {
		FGMutableData data = new FGMutableData(mMutableData.child(path));
		return data;
	}

	public ArrayList<Object> GetChildren() {
		ArrayList<Object> dataArray = new ArrayList<>();
		for (MutableData child : mMutableData.getChildren()) {
			dataArray.add(new FGMutableData(child));
		}

		return dataArray;
	}

	public long GetChildrenCount() {
		return mMutableData.getChildrenCount();
	}

	public boolean HasChild(String path) {
		return mMutableData.hasChild(path);
	}

	public boolean HasChildren() {
		return mMutableData.hasChildren();
	}

}
