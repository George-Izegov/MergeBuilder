#include "FGUtils.h"
#include "FirebaseGoodiesLog.h"
TArray<char*> StringCopies;

char* FGUtils::GetStringCopy(const FString& Value)
{
	char* Destination = new char[Value.Len() + 1];

	FCStringAnsi::Strcpy(Destination, Value.Len() + 1, TCHAR_TO_UTF8(*Value));

	StringCopies.Add(Destination);

	return Destination;
}

void FGUtils::ClearStringCopies()
{
	for (const auto Str : StringCopies)
	{
		delete[]Str;
	}

	StringCopies.Empty();
}
