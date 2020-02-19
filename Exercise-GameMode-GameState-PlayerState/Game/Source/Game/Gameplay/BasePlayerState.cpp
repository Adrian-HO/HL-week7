// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "BasePlayerState.h"
#include "Net/UnrealNetwork.h"

//TODO Week 7: REPLICATE Variables
void ABasePlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
   
}

void ABasePlayerState::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}
