// Copyright © 2010-2020 Singular Inc. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SingularLinkParams.h"
#include "SingularDelegates.generated.h"

// Dynamic delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConversionValueUpdated, int, conversionValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSingularLinksHandler, const FSingularLinkParams&, LinkParams);

UCLASS( ClassGroup=(SingularSDK), meta=(BlueprintSpawnableComponent) )
class SINGULARSDK_API USingularDelegates : public UActorComponent
{
    GENERATED_BODY()
    
public:
    USingularDelegates(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(BlueprintAssignable, Category = "Singular-SDK")
    FOnConversionValueUpdated OnConversionValueUpdated;

    UPROPERTY(BlueprintAssignable, Category = "Singular-SDK")
    FSingularLinksHandler OnSingularLinksResolved;
};
