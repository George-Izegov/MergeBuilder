// Copyright (c) 2022 Nineva Studios

package com.ninevastudios.unrealfirebase;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;

import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.tasks.Task;
import com.google.firebase.auth.GoogleAuthProvider;

public class FGAuthIntermediateActivity extends Activity {

	public static final String EXTRA_ACTION_REQUIRED = "extra_action_required";
	public static final String EXTRA_FIREBASE_CLIENT_ID = "firebase_client_id";

	public static final int NO_ACTION = -1;
	public static final int SIGN_IN_GOOGLE = 1;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		int actionCode = getIntent().getIntExtra(EXTRA_ACTION_REQUIRED, NO_ACTION);

		if (actionCode == NO_ACTION) {
			finish();
			return;
		}

		switch (actionCode) {
			case SIGN_IN_GOOGLE:
				String clientId = getIntent().getStringExtra(EXTRA_FIREBASE_CLIENT_ID);
				signInWithGoogle(clientId);
				break;
		}
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent intent) {
		super.onActivityResult(requestCode, resultCode, intent);

		if (requestCode == SIGN_IN_GOOGLE) {
			handleGoogleSignIn(intent);
		}

		finish();
	}

	private void signInWithGoogle(String clientID) {
		GoogleSignInOptions gso = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
				.requestIdToken(clientID)
				.requestEmail()
				.build();

		GoogleSignInClient mGoogleSignInClient = GoogleSignIn.getClient(this, gso);
		Intent signInIntent = mGoogleSignInClient.getSignInIntent();
		startActivityForResult(signInIntent, SIGN_IN_GOOGLE);
	}

	private void handleGoogleSignIn(Intent intent) {
		Task<GoogleSignInAccount> task = GoogleSignIn.getSignedInAccountFromIntent(intent);
		try {
			GoogleSignInAccount account = task.getResult(ApiException.class);
			if (account != null) {
				FGAuth.OnGoogleSignInSuccess(GoogleAuthProvider.getCredential(account.getIdToken(), null));
			} else {
				FGAuth.OnAuthError("GoogleSignInAccount is not valid for credentials creation.");
			}
		} catch (ApiException e) {
			FGAuth.OnAuthError("Google sign in error: " + e.getStatusCode() + " - " + e.getMessage());
		}
	}
}
