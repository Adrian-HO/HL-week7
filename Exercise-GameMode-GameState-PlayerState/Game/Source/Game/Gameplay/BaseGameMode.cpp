// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "BaseGameMode.h"
#include "Characters/CharacterBase.h"

//TODO Week 7: Override from AGameModeBase, called when a new player logs in
void ABaseGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
    //CALL Super Function
    
    //CALL HandleNewPlayer() pass in NewPlayer
    
}

//TODO Week 7: Respawn the player, Called from ACharacterBase when a player is killed
void ABaseGameMode::RespawnPlayer(APlayerController* NewPlayer, int playerTeam, int NetIndex)
{
    //DECLARE a TArray of type AActor* called PlayerStarts
    
    //CALL UGameplayStatics::GetAllActorsOfClass() and pass in GetWorld(), APlayerStart::StaticClass(), PlayerStarts to populate the PlayerStarts TArray
    
    //TArray<AActor*> PreferredStarts;

    //DECLARE a APawn* called pawn and assign it to the return value of SpawnDefaultPawnFor(NewPlayer, PlayerStarts[0]), Look up this Function in the documentation
    
    //IF the pawn is not nullptr    
    {
        //IF the pawn is a ACharacterBase type
        {
            //SET the pawn's playerTeam to the playerTeam passed in

            //SET the pawn's NetIndex to the NetIndex passed in

            //CALL SetPawn() on the NewPlayer and pass in pawn

            //CALL RestartPlayer() and pass in NewPlayer ..Look up the RestartPlayer() function in the documentation            
        }
        //ENDIF
    }
    //ENDIF
}

//TODO Week 7: Handle the new player that has logged in, assign team and net index
void ABaseGameMode::HandleNewPlayer(APlayerController* NewPlayer)
{
    //DECLARE a ACharacterBase* called character and assign it to the Cast of NewPlayer->GetPawn()
    
    //IF the character is not nullptr    
    {
        //Draw a debug message saying character has logged in

        //CALL AssignTeams() on the character

        //CALL AssignNetIndex() on the character

    }
    //ENDIF
}

