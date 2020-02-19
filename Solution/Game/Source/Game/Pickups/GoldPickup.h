// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/PickupActor.h"
#include "GoldPickup.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AGoldPickup : public APickupActor
{
	GENERATED_BODY()
	
public:
	AGoldPickup();
private:

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
	
};
