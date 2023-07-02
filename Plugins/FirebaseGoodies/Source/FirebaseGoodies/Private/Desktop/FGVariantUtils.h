// Copyright (c) 2021 Nineva Studios

#pragma once

#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
#include "firebase/variant.h"
#include "firebase/firestore.h"
#include "Common/FGValueVariant.h"
#endif

namespace FGVariantUtils
{
#if (PLATFORM_WINDOWS || PLATFORM_MAC) && FG_ENABLE_EDITOR_SUPPORT
	firebase::Variant ValueVariantToFirebaseVariant(const FFGValueVariant& Variant);
	firebase::Variant ValueVariantMapToFirebaseVariant(const TMap<FString, FFGValueVariant>& Values);

	FFGValueVariant FirebaseVariantToValueVariant(const firebase::Variant& Variant);

	TArray<FFGValueVariant> FirebaseVariantVectorToValueVariantArray(const std::vector<firebase::Variant>& Vector);
	TMap<FString, FFGValueVariant> FirebaseVariantMapToValueVariantMap(const std::map<firebase::Variant, firebase::Variant>& Map);

	firebase::firestore::FieldValue ValueVariantToFirebaseFieldValue(const FFGValueVariant& Variant);
	firebase::firestore::MapFieldValue ValueVariantMapToFirebaseMapFieldValue(const TMap<FString, FFGValueVariant>& Values);
	std::vector<firebase::firestore::FieldValue> ValueVariantArrayToFirebaseFieldValueVector(const TArray<FFGValueVariant>& Values);

	FFGValueVariant FirebaseFieldValueToValueVariant(const firebase::firestore::FieldValue& Variant);
	TArray<FFGValueVariant> FirebaseFieldValueVectorToValueVariantArray(const std::vector<firebase::firestore::FieldValue>& Vector);
	TMap<FString, FFGValueVariant> FirebaseMapFieldValueToValueVariantMap(const firebase::firestore::MapFieldValue& Values);
#endif
}