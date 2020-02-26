// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "BaseGameMode.h"
#include "Characters/CharacterBase.h"

//TODO Week 7: Override from AGameModeBase, called when a new player logs in
void ABaseGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	//CALL Super Function
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	//CALL HandleNewPlayer() pass in NewPlayer
	HandleNewPlayer(NewPlayer);
}

//TODO Week 7: Respawn the player, Called from ACharacterBase when a player is killed
void ABaseGameMode::RespawnPlayer(APlayerController* NewPlayer, int playerTeam, int NetIndex)
{
	//DECLARE a TArray of type AActor* called PlayerStarts
	TArray<AActor*> PlayerStarts;
	//CALL UGameplayStatics::GetAllActorsOfClass() and pass in GetWorld(), APlayerStart::StaticClass(), PlayerStarts to populate the PlayerStarts TArray
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
	//TArray<AActor*> PreferredStarts;

	//DECLARE a APawn* called pawn and assign it to the return value of SpawnDefaultPawnFor(NewPlayer, PlayerStarts[0]), Look up this Function in the documentation
	APawn* pawn = SpawnDefaultPawnFor(NewPlayer, PlayerStarts[0]);
	//IF the pawn is not nullptr   
	if (pawn)
	{
		//IF the pawn is a ACharacterBase type
		if (Cast<ACharacterBase>(pawn))
		{
			//SET the pawn's playerTeam to the playerTeam passed in
			Cast<ACharacterBase>(pawn)->playerTeam = playerTeam;
			//SET the pawn's NetIndex to the NetIndex passed in
			Cast<ACharacterBase>(pawn)->NetIndex = NetIndex;
			//CALL SetPawn() on the NewPlayer and pass in pawn
			NewPlayer->SetPawn(pawn);
			//CALL RestartPlayer() and pass in NewPlayer ..Look up the RestartPlayer() function in the documentation     
			RestartPlayer(NewPlayer);
		}
	}
}

//TODO Week 7: Handle the new player that has logged in, assign team and net index
void ABaseGameMode::HandleNewPlayer(APlayerController* NewPlayer)
{
	//DECLARE a ACharacterBase* called character and assign it to the Cast of NewPlayer->GetPawn()
	ACharacterBase* character = Cast<ACharacterBase>(NewPlayer->GetPawn());
	//IF the character is not nullptr    
	if (character)
	{
		//Draw a debug message saying character has logged in
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, "CHaracter Logged In");
		//CALL AssignTeams() on the character
		character->AssignTeams();
		//CALL AssignNetIndex() on the character
		character->AssignNetIndex();
	}
	//ENDIF
}

