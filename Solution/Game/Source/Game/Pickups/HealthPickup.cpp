// Fill out your copyright notice in the Description page of Project Settings.
#include "Game.h"
#include "HealthPickup.h"

AHealthPickup::AHealthPickup() : Super()
{
    Tags.Add("Health");
}

void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();
}

void AHealthPickup::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
