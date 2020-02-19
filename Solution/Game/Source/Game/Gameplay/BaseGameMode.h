// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include "GameFramework/GameMode.h"
#include "GameFramework/PlayerStart.h"
#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BaseGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API ABaseGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
   
    //TODO Week 7: Team One's Score
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
        int TeamOneScore = 0;

    //TODO Week 7:Team Two's Score
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
        int TeamTwoScore = 0;

    //TODO Week 7: Override from GameMode to handle when a new player logs in
    virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

    //TODO Week 7: Respawn the player that has died
    void RespawnPlayer(APlayerController* NewPlayer, int playerTeam, int NetIndex);

private:
    //TODO Week 7: Handle the new player
    void HandleNewPlayer(APlayerController* NewPlayer);

};
