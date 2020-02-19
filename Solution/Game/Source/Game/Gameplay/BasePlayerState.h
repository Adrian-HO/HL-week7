// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BasePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API ABasePlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

public:

    UFUNCTION()
        virtual void Tick(float DeltaSeconds) override;

    //TODO Week 7: Should all be pretty self-explanatory
    UPROPERTY(VisibleAnywhere)
        float GameTime;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        FString LastDamagedByName;

    UPROPERTY()
        float RespawnTimer;

    UPROPERTY(VisibleAnywhere)
        int PlayerHealth;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        int Deaths;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        int MatchDeaths;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        int Kills;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        int MatchKills;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        int Index;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        int playerScore;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        int MatchScore;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        int shotsFiredThisRound;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        int shotsHitThisRound;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
        int XPGained;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
        int PlayerTeam;
	
};
