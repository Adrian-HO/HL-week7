// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "WeaponBase.h"

#include <Components/SkeletalMeshComponent.h>
#include <Components/PrimitiveComponent.h>
#include <Components/ArrowComponent.h>
#include "Characters/CharacterBase.h"
#include "Net/UnrealNetwork.h"

AWeaponBase::AWeaponBase() :
	Primitive(nullptr),
	Muzzle(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	// Set default variables.
	FireRate = 10.0f;

	MaximumAmmo = 30;
	CurrentAmmo = -1;

    //TODO: Set Weapon as a replicated Actor
	//NET CODE 1 NET PICKUP
    //CALL SetReplicates() and pass in true
	SetReplicates(true);
    //CALL SetReplicateMovement() and pass in true
	SetReplicateMovement(true);

}

void AWeaponBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Get the root primitive component.
	Primitive = Cast<UPrimitiveComponent>(GetRootComponent());
	//check(Primitive != nullptr && "Actor is mising a primitive component!");

	// Get the muzzle arrow component.
	auto Components = GetComponents();
	for(auto Component : Components)
	{
		if(Component->GetFName() == "Muzzle")
		{
			Muzzle = Cast<UArrowComponent>(Component);
			break;
		}
	}

	//check(Muzzle != nullptr && "Actor is mising an arrow component for muzzle!");

	// Sanitize default values.
	MaximumAmmo = FMath::Max(1, MaximumAmmo);
	CurrentAmmo = FMath::Min(CurrentAmmo, MaximumAmmo);

	// Set current ammo to maximum ammo.
	if(CurrentAmmo < 0)
	{
		CurrentAmmo = MaximumAmmo;
	}
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AWeaponBase::SpawnProjectile()
{
    if (CurrentAmmo > 0)
    {
        // Spawn a projectile.
        FActorSpawnParameters ActorSpawnParams;
        ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
        ActorSpawnParams.Owner = this;

        ActorSpawnParams.Instigator = Cast<APawn>(GetOwner());
        FTransform Transform = Muzzle->GetComponentToWorld();
        GetWorld()->SpawnActor<AActor>(ProjectileType, Transform.GetLocation(), Transform.GetRotation().Rotator(), ActorSpawnParams);

        // Broadcast a weapon fired event.
        OnWeaponFired.Broadcast();

        // Decrement the ammo count.
        CurrentAmmo -= 1;
    }
}

void AWeaponBase::ReplenishAmmo(int ammount) //_Implementation
{
    CurrentAmmo += ammount;
    if (CurrentAmmo > MaximumAmmo)
        CurrentAmmo = MaximumAmmo;

    Multicast_ReplenishAmmo(CurrentAmmo);
}

void AWeaponBase::Multicast_ReplenishAmmo_Implementation(int ammo)
{
    CurrentAmmo = ammo;
}

void AWeaponBase::Attach(class ACharacterBase* Character)
{
	verify(Character != nullptr && "Weapon's attach function called with null character!");

	// Set owner of this weapon.
	SetOwner(Character);

    //TODO: Disable physics on the server if this Actor is on the server
   //NET CODE 1 NET PICKUP
   // Disable weapon's physics.
   //IF Role is ROLE_Authority
	if (GetLocalRole() == ROLE_Authority)
	{
        //CALL SetActorEnableCollision() and set it to false
		SetActorEnableCollision(false);
        //CALL SetSimulatePhysics() and set it to false
		Primitive->SetSimulatePhysics(false);
	}
    //ENDIF
	// Attach weapon to the character's mesh.
	AttachToComponent(Character->GetSkeletalMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, "WeaponSocket");
}

void AWeaponBase::Detach()
{
	// Stop firing when the weapon is detached.
	ReleaseTrigger();

	// Unset owner of the weapon.
	SetOwner(nullptr);

    //TODO: Enable physics on the server if this Actor is on the server
    //NET CODE 1 NET PICKUP
    // Re-enable weapon's physics.
    //IF Role is ROLE_Authority
	if (GetLocalRole() == ROLE_Authority)
	{
        //CALL SetActorEnableCollision() and set it to true
		SetActorEnableCollision(true);
        //CALL SetSimulatePhysics() and set it to true
		Primitive->SetSimulatePhysics(true);
	}
    //ENDIF
	// Detach weapon from the character's mesh.
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void AWeaponBase::PullTrigger()
{
	auto& TimerManager = GetWorld()->GetTimerManager();

	// Check current ammo value before attempting to firing.
	if(CurrentAmmo > 0)
	{
		// Start the firing timer and use the remaining time of the previous timer.
		float RemainingTime = FMath::Max(TimerManager.GetTimerRemaining(FireTimer), 0.0f);
		TimerManager.SetTimer(FireTimer, this, &AWeaponBase::Fire, 1.0f / FireRate, true, RemainingTime);
	}
}

void AWeaponBase::ReleaseTrigger()
{
	auto& TimerManager = GetWorld()->GetTimerManager();

	// Replace timer with one that will prevent the primary fire timer from triggering again too quickly.
	if(TimerManager.TimerExists(FireTimer))
	{
		float RemainingTime = TimerManager.GetTimerRemaining(FireTimer);
		TimerManager.SetTimer(FireTimer, this, &AWeaponBase::ClearFireTimer, RemainingTime, false);
	}
}

void AWeaponBase::Fire()
{
    //TODO: Shoot on the server
    //CALL ServerFire()
    ServerFire();

}

void AWeaponBase::NetMulticastFire_Implementation()
{
    //TODO: Fire on Clients
    //IF Role is LESS THAN ROLE_Authority
    if (GetLocalRole() < ROLE_Authority)
    {
        //CALL SpawnProjectile()
        SpawnProjectile();
    }
    //ENDIF

}

//NET CODE 2
void AWeaponBase::ServerFire_Implementation()
{
    //TODO: CHeck what mode we are in and fire
	//CAN BE BETTER. Change Projectile and how its spawned. More Custom built Projectile (Revisit)
        //IF The Network Mode is ENetMode::NM_DedicatedServer
		//if (GetNetMode() == ENetMode::NM_DedicatedServer)
  //          //CALL NetMulticastFire()
		//	NetMulticastFire();
  //      //ENDIF

  //      //IF The Network Mode is NOT ENetMode::NM_DedicatedServer
  //      if (GetNetMode() != ENetMode::NM_DedicatedServer)
            //CALL SpawnProjectile()
            SpawnProjectile();
        //ENDIF
		
    //Fire();
    //SpawnProjectile();
}
//NET CODE 2 //for anti cheat. returning false disconnects the client
bool AWeaponBase::ServerFire_Validate()
{
	return true;
}
void AWeaponBase::ClearFireTimer()
{
	// Clear the timer after a delay set in ReleaseTrigger() function.
	GetWorld()->GetTimerManager().ClearTimer(FireTimer);
}

FVector AWeaponBase::GetMuzzleLocation() const
{
	return Muzzle->GetComponentToWorld().GetLocation();
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    //TODO: Replicate variables
	//Don't want to replicate to client that owns this weapon
	//DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
	DOREPLIFETIME(AWeaponBase, FireRate);
	DOREPLIFETIME(AWeaponBase, MaximumAmmo);
	DOREPLIFETIME(AWeaponBase, CurrentAmmo);

}
