// Copyright (c) 2022 Nineva Studios

#include "FGCloudFunctionsErrorUtils.h"
#include "FirebaseGoodiesLog.h"

namespace CloudFunctionsErrorUtils
{

NSString* FunctionErrorCodeToString(FIRFunctionsErrorCode Code)
{
	NSString* result;
	switch (Code)
	{
		case FIRFunctionsErrorCodeOK:
			result = @"FIRFunctionsErrorCodeOK";
			break;
		case FIRFunctionsErrorCodeCancelled:
			result = @"FIRFunctionsErrorCodeCancelled";
			break;
		case FIRFunctionsErrorCodeUnknown:
			result = @"FIRFunctionsErrorCodeUnknown";
			break;
		case FIRFunctionsErrorCodeInvalidArgument:
			result = @"FIRFunctionsErrorCodeInvalidArgument";
			break;
		case FIRFunctionsErrorCodeDeadlineExceeded:
			result = @"FIRFunctionsErrorCodeDeadlineExceeded";
			break;
		case FIRFunctionsErrorCodeAlreadyExists:
			result = @"FIRFunctionsErrorCodeAlreadyExists";
			break;
		case FIRFunctionsErrorCodeResourceExhausted:
			result = @"FIRFunctionsErrorCodeResourceExhausted";
			break;
		case FIRFunctionsErrorCodeFailedPrecondition:
			result = @"FIRFunctionsErrorCodeFailedPrecondition";
			break;
		case FIRFunctionsErrorCodeAborted:
			result = @"FIRFunctionsErrorCodeAborted";
			break;
		case FIRFunctionsErrorCodeOutOfRange:
			result = @"FIRFunctionsErrorCodeOutOfRange";
			break;
		case FIRFunctionsErrorCodeUnimplemented:
			result = @"FIRFunctionsErrorCodeUnimplemented";
			break;
		case FIRFunctionsErrorCodeInternal:
			result = @"FIRFunctionsErrorCodeInternal";
			break;
		case FIRFunctionsErrorCodeUnavailable:
			result = @"FIRFunctionsErrorCodeUnavailable";
			break;
		case FIRFunctionsErrorCodeDataLoss:
			result = @"FIRFunctionsErrorCodeDataLoss";
			break;
		case FIRFunctionsErrorCodeUnauthenticated:
			result = @"FIRFunctionsErrorCodeUnauthenticated";
			break;
		default:
			result = @"FIRFunctionsErrorCodeUnknown";
	}

	return result;
}

void HandleError(NSError* error)
{
	NSString* message;
	if (error.domain == FIRFunctionsErrorDomain)
	{
		NSString* codeString = FunctionErrorCodeToString((FIRFunctionsErrorCode)error.code);
		message = [NSString stringWithFormat:@"%@: %@", codeString, error.localizedDescription];
	}
	else
	{
		message = error.localizedDescription;
	}

	FString ErrorMessage = FString(message);
	UE_LOG(LogFirebaseGoodies, Error, TEXT("Cloud Function Error: %s"), *ErrorMessage);
	AsyncTask(ENamedThreads::GameThread, [=]() {
		UFGCloudFunctions::OnCallCloudFunctionErrorCallback.ExecuteIfBound(ErrorMessage);
	});
}

}
