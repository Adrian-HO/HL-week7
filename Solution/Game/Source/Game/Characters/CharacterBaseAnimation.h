// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include <Animation/AnimInstance.h>
#include "CharacterBaseAnimation.generated.h"

UCLASS()
class GAME_API UCharacterBaseAnimation : public UAnimInstance
{
	GENERATED_BODY()

public:
    //TODO: Replicate this variable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bHasWeapon;

    //TODO: Replicate this variable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bIsAiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsMoving;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float MovementSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float StrafingRotation;

protected:
	UCharacterBaseAnimation();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

protected:
	APawn* OwningPawn;
};
