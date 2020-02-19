// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "CharacterBaseAnimation.h"
//TODO: Include this header file
#include "Net/UnrealNetwork.h"
UCharacterBaseAnimation::UCharacterBaseAnimation() :
	bHasWeapon(false),
	bIsAiming(false),
	bIsMoving(false),
	MovementSpeed(0.0f),
	StrafingRotation(0.0f),
	OwningPawn(nullptr)
{
}

void UCharacterBaseAnimation::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwningPawn = TryGetPawnOwner();
}

void UCharacterBaseAnimation::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
}

//TODO: Implement GetLifetimeReplicatedProps(...)
void UCharacterBaseAnimation::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//Don't want to replicate to client that owns this weapon
	//DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
	DOREPLIFETIME(UCharacterBaseAnimation, bHasWeapon);
	DOREPLIFETIME(UCharacterBaseAnimation, bIsAiming);

}