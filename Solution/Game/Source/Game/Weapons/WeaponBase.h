// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include <GameFramework/Actor.h>
#include "Items/ItemBase.h"
#include "WeaponBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponFiredSignature);

UCLASS()
class GAME_API AWeaponBase : public AItemBase
{
	GENERATED_BODY()

public:
    //TODO: Mark this property as Replicated
	UPROPERTY(Replicated, EditDefaultsOnly)
	float FireRate;

    //TODO: Mark this property as Replicated
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly)
	int MaximumAmmo;

    //TODO: Mark this property as Replicated
	/*UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly)
	int CurrentAmmo;*/
    UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly)
        int CurrentAmmo;

	UPROPERTY(EditDefaultsOnly)
	class UClass* ProjectileType;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponFiredSignature OnWeaponFired;

    //UFUNCTION(NetMulticast, Reliable)
    void ReplenishAmmo(int ammount);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_ReplenishAmmo(int ammo);
	
public:
	void Attach(class ACharacterBase* Character);
	void Detach();

	void PullTrigger();
	void ReleaseTrigger();

	FVector GetMuzzleLocation() const;

protected:
	AWeaponBase();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

    virtual void SpawnProjectile();

	void Fire();

    //TODO: Declare a function called ServerFire and mark it as  this property Server, Reliable, WithValidation
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerFire();

    //TODO: Declare a function called NetMulticastFire and mark it as  this property as Client, Reliable. This is used in Dedicated server mode
	UFUNCTION(NetMulticast, Reliable)	
		void NetMulticastFire();

private:
	void ClearFireTimer();
	FTimerHandle FireTimer;

private:
	class UPrimitiveComponent* Primitive;
	class UArrowComponent* Muzzle;
};
