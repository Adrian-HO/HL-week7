// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BPFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API UBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/** Returns true if this game is currently running in standalone mode. */
	UFUNCTION(BlueprintPure, Category = "Engine")
		static bool IsStandalone();

	/** Return the world type as a string (Client, ListenServer, DedicatedServer, Standalone, or nullptr) */
	UFUNCTION(BlueprintPure, Category = "Engine")
		static FString GetWorldType();
	
	
};
