// Fill out your copyright notice in the Description page of Project Settings.
#include "Game.h"
#include "AmmoPickup.h"

AAmmoPickup::AAmmoPickup() : Super()
{
    Tags.Add("Ammo");
}

void AAmmoPickup::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
}

void AAmmoPickup::BeginPlay()
{
	Super::BeginPlay();
}

void AAmmoPickup::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
