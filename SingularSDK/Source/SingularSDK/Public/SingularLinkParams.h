#pragma once
#include "SingularLinkParams.generated.h"

USTRUCT(BlueprintType)
struct FSingularLinkParams
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(Category = SingularSDK, VisibleAnywhere, BlueprintReadOnly)
	TMap<FString, FString> SingularDDLParams;
};