// Copyright (c) 2022 Nineva Studios

package com.ninevastudios.unrealfirebase;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;

import com.google.android.gms.tasks.OnCompleteListener;
import com.google.android.gms.tasks.Task;
import com.google.firebase.FirebaseException;
import com.google.firebase.auth.AuthCredential;
import com.google.firebase.auth.AuthResult;
import com.google.firebase.auth.EmailAuthProvider;
import com.google.firebase.auth.FacebookAuthProvider;
import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.auth.FirebaseUser;
import com.google.firebase.auth.GetTokenResult;
import com.google.firebase.auth.GithubAuthProvider;
import com.google.firebase.auth.GoogleAuthProvider;
import com.google.firebase.auth.OAuthProvider;
import com.google.firebase.auth.PhoneAuthCredential;
import com.google.firebase.auth.PhoneAuthProvider;
import com.google.firebase.auth.PlayGamesAuthProvider;
import com.google.firebase.auth.SignInMethodQueryResult;
import com.google.firebase.auth.TwitterAuthProvider;
import com.google.firebase.auth.UserInfo;
import com.google.firebase.auth.UserProfileChangeRequest;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.TimeUnit;

@SuppressWarnings("unused")
public class FGAuth {
	public static native void OnIdTokenChanged();

	public static native void OnAuthStateChanged();

	public static native void OnUserCreated(FirebaseUser result);

	public static native void OnAuthError(String error);

	public static native void OnSignInMethodsFetched(String[] methods);

	public static native void OnPasswordResetSent();

	public static native void OnSignInSuccess(FirebaseUser result);

	public static native void OnPhoneVerified(AuthCredential credential);

	public static native void OnPhoneVerificationCodeSent(String s);

	public static native void OnPhoneVerificationTimeout(String s);

	public static native void OnUserVoidTaskCompleted(String uid);

	public static native void OnUserUpdated(FirebaseUser user);

	public static native void OnUserOperationError(String uid, String error);

	public static native void OnUserStringTaskCompleted(String uid, String result);

	public static native void OnGoogleSignInSuccess(AuthCredential credential);

	public static void initListeners() {
		FirebaseAuth.getInstance().addIdTokenListener(new FirebaseAuth.IdTokenListener() {
			@Override
			public void onIdTokenChanged(FirebaseAuth firebaseAuth) {
				OnIdTokenChanged();
			}
		});

		FirebaseAuth.getInstance().addAuthStateListener(new FirebaseAuth.AuthStateListener() {
			@Override
			public void onAuthStateChanged(FirebaseAuth firebaseAuth) {
				OnAuthStateChanged();
			}
		});
	}

	public static FirebaseUser currentUser() {
		return FirebaseAuth.getInstance().getCurrentUser();
	}


	public static void createUser(String email, String password, Activity context) {
		FirebaseAuth.getInstance().createUserWithEmailAndPassword(email.trim(), password).addOnCompleteListener(context, new OnCompleteListener<AuthResult>() {
			@Override
			public void onComplete(Task<AuthResult> task) {
				if (task.isSuccessful()) {
					OnUserCreated(task.getResult().getUser());
				} else {
					OnAuthError("FirebaseGoodies => User creation failed: " + task.getException().toString());
				}
			}
		});
	}

	public static void fetchSignInMethods(String email) {
		FirebaseAuth.getInstance().fetchSignInMethodsForEmail (email.trim()).addOnCompleteListener(new OnCompleteListener<SignInMethodQueryResult>() {
			@Override
			public void onComplete(Task<SignInMethodQueryResult> task) {
				if (task.isSuccessful()) {
					List<String> result = task.getResult().getSignInMethods();
					OnSignInMethodsFetched(result.toArray(new String[0]));
				} else {
					OnAuthError("FirebaseGoodies => FetchSignInMethods failed: " + task.getException().toString());
				}
			}
		});
	}

	public static void sendPasswordReset(String email) {
		FirebaseAuth.getInstance().sendPasswordResetEmail(email.trim()).addOnCompleteListener(new OnCompleteListener<Void>() {
			@Override
			public void onComplete(Task<Void> task) {
				if (task.isSuccessful()) {
					OnPasswordResetSent();
				} else {
					OnAuthError("FirebaseGoodies => SendPasswordReset failed: " + task.getException().toString());
				}
			}
		});
	}

	public static void signInWithEmail(String email, String password) {
		FirebaseAuth.getInstance().signInWithEmailAndPassword(email.trim(), password).addOnCompleteListener(getSignInListener());
	}

	public static void signInWithToken(String token) {
		FirebaseAuth.getInstance().signInWithCustomToken(token.trim()).addOnCompleteListener(getSignInListener());
	}

	private static void signInWithCredential(AuthCredential credential) {
		if (credential == null) {
			return;
		}
		FirebaseAuth.getInstance().signInWithCredential(credential).addOnCompleteListener(getSignInListener());
	}

	public static AuthCredential getEmailCredential(String email, String password) {
		return EmailAuthProvider.getCredential(email, password);
	}

	public static AuthCredential getFacebookCredential(String token) {
		return FacebookAuthProvider.getCredential(token);
	}

	public static AuthCredential getGithubCredential(String token) {
		return GithubAuthProvider.getCredential(token);
	}

	public static AuthCredential getGoogleCredential(String idToken, String accessToken) {
		return GoogleAuthProvider.getCredential(idToken, accessToken);
	}

	public static AuthCredential getGenericOAuthCredential(String providerId, String idToken, String accessToken) {
		return OAuthProvider.getCredential(providerId, idToken, accessToken);
	}

	public static AuthCredential getPlayGamesCredential(String serverAuthCode) {
		return PlayGamesAuthProvider.getCredential(serverAuthCode);
	}

	public static AuthCredential getTwitterCredential(String token, String secret) {
		return TwitterAuthProvider.getCredential(token, secret);
	}

	public static AuthCredential getPhoneCredential(String verificationId, String verificationCode) {
		return PhoneAuthProvider.getCredential(verificationId, verificationCode);
	}

	public static void signInAnonymously() {
		FirebaseAuth.getInstance().signInAnonymously().addOnCompleteListener(getSignInListener());
	}

	public static void signOut() {
		FirebaseAuth.getInstance().signOut();
	}

	public static String getUserPhotoUrl(FirebaseUser user) {
		Uri photoUrl = user.getPhotoUrl();
		return photoUrl == null ? "" : photoUrl.toString();
	}

	public static String getUserInfoPhotoUrl(UserInfo userInfo) {
		Uri photoUrl = userInfo.getPhotoUrl();
		return photoUrl == null ? "" : photoUrl.toString();
	}

	public static void verifyPhoneNumber(String phoneNumber, int timeoutMs, Activity context) {
		PhoneAuthProvider.getInstance(FirebaseAuth.getInstance()).verifyPhoneNumber(
				phoneNumber, timeoutMs, TimeUnit.MILLISECONDS, context,
				new PhoneAuthProvider.OnVerificationStateChangedCallbacks() {
					@Override
					public void onVerificationCompleted(PhoneAuthCredential phoneAuthCredential) {
						OnPhoneVerified(phoneAuthCredential);
					}

					@Override
					public void onVerificationFailed(FirebaseException e) {
						OnAuthError(e.toString());
					}

					@Override
					public void onCodeAutoRetrievalTimeOut(String s) {
						OnPhoneVerificationTimeout(s);
					}

					@Override
					public void onCodeSent(String s, PhoneAuthProvider.ForceResendingToken forceResendingToken) {
						OnPhoneVerificationCodeSent(s);
					}
				});
	}

	public static void deleteUser(final FirebaseUser user) {
		user.delete().addOnCompleteListener(new OnCompleteListener<Void>() {
			@Override
			public void onComplete(Task<Void> task) {
				if (task.isSuccessful()) {
					OnUserVoidTaskCompleted(user.getUid());
				} else {
					OnUserOperationError(user.getUid(), "FirebaseGoodies => DeleteUser failed: " + task.getException().toString());
				}
			}
		});
	}

	public static void linkUserWithCredential(final FirebaseUser user, AuthCredential credential) {
		user.linkWithCredential(credential).addOnCompleteListener(new OnCompleteListener<AuthResult>() {
			@Override
			public void onComplete(Task<AuthResult> task) {
				if (task.isSuccessful()) {
					OnUserUpdated(task.getResult().getUser());
				} else {
					OnUserOperationError(user.getUid(), "FirebaseGoodies => linkUserWithCredential failed: " + task.getException().toString());
				}
			}
		});
	}

	public static void reauthenticateUser(final FirebaseUser user, AuthCredential credential) {
		user.reauthenticate(credential).addOnCompleteListener(new OnCompleteListener<Void>() {
			@Override
			public void onComplete(Task<Void> task) {
				if (task.isSuccessful()) {
					OnUserVoidTaskCompleted(user.getUid());
				} else {
					OnUserOperationError(user.getUid(), "FirebaseGoodies => reauthenticateUser failed: " + task.getException().toString());
				}
			}
		});
	}

	public static void reauthenticateUserAndRetrieveData(final FirebaseUser user, AuthCredential credential) {
		user.reauthenticateAndRetrieveData(credential).addOnCompleteListener(new OnCompleteListener<AuthResult>() {
			@Override
			public void onComplete(Task<AuthResult> task) {
				if (task.isSuccessful()) {
					OnUserUpdated(task.getResult().getUser());
				} else {
					OnUserOperationError(user.getUid(), "FirebaseGoodies => reauthenticateUserAndRetrieveData failed: " + task.getException().toString());
				}
			}
		});
	}

	public static void reloadUser(final FirebaseUser user) {
		user.reload().addOnCompleteListener(new OnCompleteListener<Void>() {
			@Override
			public void onComplete(Task<Void> task) {
				if (task.isSuccessful()) {
					OnUserVoidTaskCompleted(user.getUid());
				} else {
					OnUserOperationError(user.getUid(), "FirebaseGoodies => reloadUser failed: " + task.getException().toString());
				}
			}
		});
	}

	public static void sendEmailVerificationForUser(final FirebaseUser user) {
		user.sendEmailVerification().addOnCompleteListener(new OnCompleteListener<Void>() {
			@Override
			public void onComplete(Task<Void> task) {
				if (task.isSuccessful()) {
					OnUserVoidTaskCompleted(user.getUid());
				} else {
					OnUserOperationError(user.getUid(), "FirebaseGoodies => sendEmailVerification failed: " + task.getException().toString());
				}
			}
		});
	}

	public static void unlinkProviderForUser(final FirebaseUser user, String provider) {
		user.unlink(provider).addOnCompleteListener(new OnCompleteListener<AuthResult>() {
			@Override
			public void onComplete(Task<AuthResult> task) {
				if (task.isSuccessful()) {
					OnUserUpdated(task.getResult().getUser());
				} else {
					OnUserOperationError(user.getUid(), "FirebaseGoodies => unlinkProviderForUser failed: " + task.getException().toString());
				}
			}
		});
	}

	public static void updateEmailForUser(final FirebaseUser user, String email) {
		user.updateEmail(email).addOnCompleteListener(new OnCompleteListener<Void>() {
			@Override
			public void onComplete(Task<Void> task) {
				if (task.isSuccessful()) {
					OnUserVoidTaskCompleted(user.getUid());
				} else {
					OnUserOperationError(user.getUid(), "FirebaseGoodies => updateEmailForUser failed: " + task.getException().toString());
				}
			}
		});
	}

	public static void updatePasswordForUser(final FirebaseUser user, String password) {
		user.updatePassword(password).addOnCompleteListener(new OnCompleteListener<Void>() {
			@Override
			public void onComplete(Task<Void> task) {
				if (task.isSuccessful()) {
					OnUserVoidTaskCompleted(user.getUid());
				} else {
					OnUserOperationError(user.getUid(), "FirebaseGoodies => updatePasswordForUser failed: " + task.getException().toString());
				}
			}
		});
	}

	public static void updatePhoneNumberForUser(final FirebaseUser user, PhoneAuthCredential credential) {
		user.updatePhoneNumber(credential).addOnCompleteListener(new OnCompleteListener<Void>() {
			@Override
			public void onComplete(Task<Void> task) {
				if (task.isSuccessful()) {
					OnUserVoidTaskCompleted(user.getUid());
				} else {
					OnUserOperationError(user.getUid(), "FirebaseGoodies => updatePhoneNumberForUser failed: " + task.getException().toString());
				}
			}
		});
	}

	public static void updateProfileForUser(final FirebaseUser user, String displayName, String avatarUrl) {
		UserProfileChangeRequest request = new UserProfileChangeRequest.Builder()
				.setDisplayName(displayName)
				.setPhotoUri(Uri.parse(avatarUrl))
				.build();

		user.updateProfile(request).addOnCompleteListener(new OnCompleteListener<Void>() {
			@Override
			public void onComplete(Task<Void> task) {
				if (task.isSuccessful()) {
					OnUserVoidTaskCompleted(user.getUid());
				} else {
					OnUserOperationError(user.getUid(), "FirebaseGoodies => updateProfileForUser failed: " + task.getException().toString());
				}
			}
		});
	}

	public static void getUserToken(final FirebaseUser user, boolean forceRefresh) {
		user.getIdToken(forceRefresh).addOnCompleteListener(new OnCompleteListener<GetTokenResult>() {
			@Override
			public void onComplete(Task<GetTokenResult> task) {
				if (task.isSuccessful()) {
					String token = task.getResult().getToken() != null ? task.getResult().getToken() : "";
					OnUserStringTaskCompleted(user.getUid(), token);
				} else {
					OnUserOperationError(user.getUid(), task.getException().toString());
				}
			}
		});
	}

	public static ArrayList<UserInfo> getUserProviderData(FirebaseUser user) {
		return new ArrayList<>(user.getProviderData());
	}

	private static OnCompleteListener<AuthResult> getSignInListener() {
		return new OnCompleteListener<AuthResult>() {
			@Override
			public void onComplete(Task<AuthResult> task) {
				if (task.isSuccessful()) {
					OnSignInSuccess(task.getResult().getUser());
				} else {
					OnAuthError("FirebaseGoodies => SignIn failed: " + task.getException().toString());
				}
			}
		};
	}

	public static void signInWithGoogle(Activity context, String clientId) {
		startAuthActivity(context, clientId, FGAuthIntermediateActivity.SIGN_IN_GOOGLE);
	}

	private static void startAuthActivity(Activity activity, String clientId, int actionType) {
		Intent intent = new Intent(activity, FGAuthIntermediateActivity.class);
		intent.putExtra(FGAuthIntermediateActivity.EXTRA_FIREBASE_CLIENT_ID, clientId);
		intent.putExtra(FGAuthIntermediateActivity.EXTRA_ACTION_REQUIRED, actionType);

		activity.startActivity(intent);
	}
}
