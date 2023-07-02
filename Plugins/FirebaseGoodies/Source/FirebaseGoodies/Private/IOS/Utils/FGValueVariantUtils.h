// Copyright (c) 2022 Nineva Studios

#pragma once

#include "Common/FGValueVariant.h"

namespace ValueVariantUtils
{
id VariantArrayToNSArray(const TArray<FFGValueVariant>& ValueArray);
id VariantMapToNSDictionary(const TMap<FString, FFGValueVariant>& ValueMap);
TArray<FFGValueVariant> NSArrayToVariantArray(NSArray* Array);
TMap<FString, FFGValueVariant> NSDictionaryToVariantMap(NSDictionary* Dict);
id VariantToId(const FFGValueVariant& Value);
FFGValueVariant IdToVariant(id Object);
}
