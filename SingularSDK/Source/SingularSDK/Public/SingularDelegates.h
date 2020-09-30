// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SingularDelegates.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConversionValueUpdated, int, conversionValue);

UCLASS( ClassGroup=(SingularSDK), meta=(BlueprintSpawnableComponent) )
class SINGULARSDK_API USingularDelegates : public UActorComponent
{
    GENERATED_BODY()
    
public:
    USingularDelegates(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(BlueprintAssignable, Category = "Singular-SDK")
    FOnConversionValueUpdated OnConversionValueUpdated;
};
