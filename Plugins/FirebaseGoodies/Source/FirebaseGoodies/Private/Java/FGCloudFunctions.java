package com.ninevastudios.unrealfirebase;

import androidx.annotation.NonNull;

import com.google.android.gms.tasks.Continuation;
import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.functions.FirebaseFunctions;
import com.google.firebase.functions.HttpsCallableResult;

import java.util.List;
import java.util.Map;

@SuppressWarnings("unused")
public class FGCloudFunctions {

	public static native void OnStringFunctionComplete(String result);

	public static native void OnFunctionError(String error);

	public static native void OnIntFunctionComplete(int result);

	public static native void OnFloatFunctionComplete(float result);

	public static native void OnBoolFunctionComplete(boolean result);

	public static native void OnVoidFunctionComplete();

	public static native void OnMapFunctionComplete(Object result);

	public static native void OnArrayFunctionComplete(Object result);

	public static void callStringFunction(String methodName, String region, Map<String, Object> parameters) {
		FirebaseFunctions.getInstance(region)
				.getHttpsCallable(methodName)
				.call(parameters)
				.continueWith(new Continuation<HttpsCallableResult, String>() {
					@Override
					public String then(@NonNull Task<HttpsCallableResult> task) {
						return (String) task.getResult().getData();
					}
				})
				.addOnCompleteListener(new OnCompleteListener<String>() {
					@Override
					public void onComplete(@NonNull Task<String> task) {
						if (!task.isSuccessful()) {
							Exception e = task.getException();
							OnFunctionError("String cloud function. An error occurred: " + e);
							return;
						}

						OnStringFunctionComplete(task.getResult());
					}
				});
	}

	public static void callIntFunction(String methodName, String region, Map<String, Object> parameters) {
	    FirebaseFunctions.getInstance(region)
                .getHttpsCallable(methodName)
                .call(parameters)
                .continueWith(new Continuation<HttpsCallableResult, Integer>() {
                    @Override
                    public Integer then (@NonNull Task<HttpsCallableResult> task) {
                        return (Integer) task.getResult().getData();
                    }
                })
                .addOnCompleteListener(new OnCompleteListener<Integer>() {
                    @Override
                    public void onComplete(@NonNull Task<Integer> task) {
                        if(!task.isSuccessful()) {
                            Exception e = task.getException();
                            OnFunctionError("Int cloud function. An error occurred: " + e);
                            return;
                        }

                        OnIntFunctionComplete(task.getResult());
                    }
                });
    }

	public static void callFloatFunction(String methodName, String region, Map<String, Object> parameters) {
		FirebaseFunctions.getInstance(region)
				.getHttpsCallable(methodName)
				.call(parameters)
				.continueWith(new Continuation<HttpsCallableResult, Double>() {
					@Override
					public Double then (@NonNull Task<HttpsCallableResult> task) {
						return (Double) task.getResult().getData();
					}
				})
				.addOnCompleteListener(new OnCompleteListener<Double>() {
					@Override
					public void onComplete(@NonNull Task<Double> task) {
						if(!task.isSuccessful()) {
							Exception e = task.getException();
							OnFunctionError("Float cloud function. An error occurred: " + e);
							return;
						}

						OnFloatFunctionComplete(task.getResult().floatValue());
					}
				});
	}

	public static void callBoolFunction(String methodName, String region, Map<String, Object> parameters) {
		FirebaseFunctions.getInstance(region)
				.getHttpsCallable(methodName)
				.call(parameters)
				.continueWith(new Continuation<HttpsCallableResult, Boolean>() {
					@Override
					public Boolean then (@NonNull Task<HttpsCallableResult> task) {
						return (Boolean) task.getResult().getData();
					}
				})
				.addOnCompleteListener(new OnCompleteListener<Boolean>() {
					@Override
					public void onComplete(@NonNull Task<Boolean> task) {
						if(!task.isSuccessful()) {
							Exception e = task.getException();
							OnFunctionError("Bool cloud function. An error occurred: " + e);
							return;
						}

						OnBoolFunctionComplete(task.getResult());
					}
				});
	}

	public static void callVoidFunction(String methodName, String region, Map<String, Object> parameters) {
		FirebaseFunctions.getInstance(region)
				.getHttpsCallable(methodName)
				.call(parameters)
				.continueWith(new Continuation<HttpsCallableResult, Void>() {
					@Override
					public Void then (@NonNull Task<HttpsCallableResult> task) {
						return null;
					}
				})
				.addOnCompleteListener(new OnCompleteListener<Void>() {
					@Override
					public void onComplete(@NonNull Task<Void> task) {
						if(!task.isSuccessful()) {
							Exception e = task.getException();
							OnFunctionError("Void cloud function. An error occurred: " + e);
							return;
						}

						OnVoidFunctionComplete();
					}
				});
	}

	public static void callMapFunction(String methodName, String region, Map<String, Object> parameters) {
		FirebaseFunctions.getInstance(region)
				.getHttpsCallable(methodName)
				.call(parameters)
				.continueWith(new Continuation<HttpsCallableResult, Object>() {
					@Override
					public Object then (@NonNull Task<HttpsCallableResult> task) {
						return task.getResult().getData();
					}
				})
				.addOnCompleteListener(new OnCompleteListener<Object>() {
					@Override
					public void onComplete(@NonNull Task<Object> task) {
						if(!task.isSuccessful()) {
							Exception e = task.getException();
							OnFunctionError("Map cloud function. An error occurred: " + e);
							return;
						}

						OnMapFunctionComplete(task.getResult());
					}
				});
	}

	public static void callArrayFunction(String methodName, String region, Map<String, Object> parameters) {
		FirebaseFunctions.getInstance(region)
				.getHttpsCallable(methodName)
				.call(parameters)
				.continueWith(new Continuation<HttpsCallableResult, Object>() {
					@Override
					public Object then (@NonNull Task<HttpsCallableResult> task) {
						return task.getResult().getData();
					}
				})
				.addOnCompleteListener(new OnCompleteListener<Object>() {
					@Override
					public void onComplete(@NonNull Task<Object> task) {
						if(!task.isSuccessful()) {
							Exception e = task.getException();
							OnFunctionError("Array cloud function. An error occurred: " + e);
							return;
						}

						OnArrayFunctionComplete(task.getResult());
					}
				});
	}
}