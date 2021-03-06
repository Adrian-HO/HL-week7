// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include <GameFramework/PlayerController.h>
#include "PlayerControllerDefault.generated.h"

UCLASS()
class GAME_API APlayerControllerDefault : public APlayerController
{
	GENERATED_BODY()
	
protected:
	APlayerControllerDefault();

	virtual void SetupInputComponent() override;
	virtual void AcknowledgePossession(APawn* Pawn) override;
	virtual void BeginPlay() override;
	
private:
	void MoveVertical(float Scale);
	void MoveHorizontal(float Scale);

	void FirePressed();
	void FireReleased();
	void AimPressed();
	void AimReleased();

	void InteractPressed();

private:
	class ACharacterBase* Character;
};
