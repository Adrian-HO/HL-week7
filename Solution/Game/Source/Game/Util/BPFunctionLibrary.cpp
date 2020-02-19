// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "BPFunctionLibrary.h"




bool UBPFunctionLibrary::IsStandalone()
{
	if (GEngine != nullptr && GWorld != nullptr)
	{
		return GEngine->GetNetMode(GWorld) == NM_Standalone;
	}
	return false;
}

FString UBPFunctionLibrary::GetWorldType()
{
	if (GEngine != nullptr && GWorld != nullptr)
	{
		return (GEngine->GetNetMode(GWorld) == NM_Client) ? TEXT("Client")
			: (GEngine->GetNetMode(GWorld) == NM_ListenServer) ? TEXT("ListenServer")
			: (GEngine->GetNetMode(GWorld) == NM_DedicatedServer) ? TEXT("DedicatedServer")
			: TEXT("Standalone");
	}
	return TEXT("nullptr");
}
