// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/PickupActor.h"
#include "KeyPickup.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AKeyPickup : public APickupActor
{
	GENERATED_BODY()
	
public:
	AKeyPickup();
private:

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
	
};
