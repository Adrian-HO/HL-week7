// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/PickupActor.h"
#include "AmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AAmmoPickup : public APickupActor
{
	GENERATED_BODY()
public:
	AAmmoPickup();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
		uint8 Capacity;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
private:

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	
};
