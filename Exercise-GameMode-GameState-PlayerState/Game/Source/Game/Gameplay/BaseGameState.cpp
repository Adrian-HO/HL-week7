// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "BaseGameState.h"
#include "Net/UnrealNetwork.h"

//TODO Week 7: REPLICATE Variables
void ABaseGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//REPLICATE Vars
	DOREPLIFETIME(ABaseGameState, TeamsEnabled);
	DOREPLIFETIME(ABaseGameState, TeamOneScore);
	DOREPLIFETIME(ABaseGameState, TeamTwoScore);

	DOREPLIFETIME(ABaseGameState, TeamOneMatchScore);
	DOREPLIFETIME(ABaseGameState, TeamTwoMatchScore);

	DOREPLIFETIME(ABaseGameState, TeamOneSize);
	DOREPLIFETIME(ABaseGameState, TeamTwoSize);
	DOREPLIFETIME(ABaseGameState, GameTime);

	DOREPLIFETIME(ABaseGameState, TeamOnePMaterials);
	DOREPLIFETIME(ABaseGameState, TeamTwoPMaterials);
}