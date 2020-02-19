// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "BasePlayerState.h"
#include "Net/UnrealNetwork.h"

//TODO Week 7: REPLICATE Variables
void ABasePlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABasePlayerState, Kills);
    DOREPLIFETIME(ABasePlayerState, MatchKills);
    DOREPLIFETIME(ABasePlayerState, Deaths);
    DOREPLIFETIME(ABasePlayerState, MatchDeaths);
    DOREPLIFETIME(ABasePlayerState, XPGained);
    DOREPLIFETIME(ABasePlayerState, playerScore); 
    DOREPLIFETIME(ABasePlayerState, PlayerTeam);
    DOREPLIFETIME(ABasePlayerState, MatchScore);
}

void ABasePlayerState::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
}
