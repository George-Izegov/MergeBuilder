// Copyright (c) 2021 Nineva Studios

package com.ninevastudios.unrealfirebase;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import androidx.annotation.NonNull;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.dynamiclinks.FirebaseDynamicLinks;
import com.google.firebase.dynamiclinks.PendingDynamicLinkData;

import com.epicgames.ue4.GameActivity;

public class FGDynamicLinksActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		final Activity activity = this;

		FirebaseDynamicLinks.getInstance().getDynamicLink(getIntent())
				.addOnCompleteListener(new OnCompleteListener<PendingDynamicLinkData>() {
					@Override
					public void onComplete(@NonNull Task<PendingDynamicLinkData> task) {
						if (task.isSuccessful()) {
							FGDynamicLinks.pendingDynamicLinkData = task.getResult();
						} else {
							Log.w("FGDynamicLinksActivity", "Failed to get dynamic link.");
						}

						Intent intent  = new Intent(activity, GameActivity.class);
						startActivity(intent);

						finish();
					}
				});
	}
}