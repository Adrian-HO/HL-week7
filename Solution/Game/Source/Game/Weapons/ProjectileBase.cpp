// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "ProjectileBase.h"
#include "Characters/CharacterBase.h"
#include "Characters/AICharacterBase.h"
AProjectileBase::AProjectileBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// Default projectile parameters.
	DamageAmount = 10.0f;
	Speed = 1200.0f;
	LifeTime = 1.0f;
    //TODO: Replicate this Actor
    //CALL SetReplicates() and pass in true
	SetReplicates(true);
    //CALL SetReplicateMovement() and pass in true
	SetReplicateMovement(true);
}

void AProjectileBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Subscribe to actor's hit event.
	AActor::OnActorHit.AddDynamic(this, &AProjectileBase::OnActorHit);
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	// Destroy the projectile after some time.
	GetWorld()->GetTimerManager().SetTimer(Timer, this, &AProjectileBase::DestroySelf, LifeTime, false);
}

void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Move the projectile in a forward direction.
	FVector Location = GetActorLocation();
	Location += GetActorForwardVector() * Speed * DeltaTime;
	SetActorLocation(Location, true);
}
//TODO: Handle the projectile Hit
//REVISIT
void AProjectileBase::OnActorHit(AActor* Self, AActor* Other, FVector NormalImpulse, const FHitResult& Hit)
{
	
	// Cause damage to the hit actor.
    //IF Other is NOT nullptr
	if(Other != nullptr)
	{		
        //IF Role is Role_Authority
		if (GetLocalRole() == ROLE_Authority)// || Role == ROLE_AutonomousProxy)
		{
            //CALL MakeNoise() and pass in 1.0f, Instigator
			MakeNoise(1.0f, GetInstigator());
            //CAST Other to a ACharacterBase* variable called CharacterB
			ACharacterBase* OtherCharacter = Cast<ACharacterBase>(Other);
            //IF CharacterBase is valid
			if (OtherCharacter)
			{
                //DECLARE a AActor* called Weapon and assign it to the return value of GetOwner()
                AActor* Weapon = GetOwner();
                //DECLARE a AActor* called Character and assign it to the return value of the Weapon's GetOwner()
                AActor* Character = Weapon->GetOwner();
                //Draw a debug Message to display the Character Name
                if(Character)
                {
                    //TODO Week 7: Only Damage player if not on same team
                    //IF Character playerTeam is NOT EQUAL to OtherCharacter playerTeam
                    if (Cast<ACharacterBase>(Character)->playerTeam != OtherCharacter->playerTeam)
                    {

                        FDamageEvent DamageEvent;
                        float fDamage = 10.0f;
                        if (OtherCharacter->ActorHasTag("AICharacter"))
                        {
                            //AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser
                            Cast<AAICharacterBase>(OtherCharacter)->ApplyDamageAI(OtherCharacter, fDamage, nullptr, Character->GetInstigatorController(), Character);
                        }
                        else
                        {
                            OtherCharacter->ServerApplyDamage(OtherCharacter, fDamage, nullptr, Character->GetInstigatorController(), Character);
                        }
                    }
                    //ENDIF
                }
                //}
			}
            //ENDIF

            //CALL Destroy()
		}
        //ENDIF
		
	}
    //ENDIF

	// Destroy self.

	//Net Code
	if (GetLocalRole() == ROLE_Authority) //Checks whether we are the server
	{
		//MakeNoise(1.0f, Instigator); //Instigator is in every Actor, usually used for Damage application i.e applying damage to other actors
									 //In this case the Instigator is the Character pawn, because it spawned this projectile
		Destroy();
	}
}

void AProjectileBase::DestroySelf()
{
	// Destroy self.
    //IF Role is Role_Authority
	if (GetLocalRole() == ROLE_Authority)
        //CALL Destroy()
		Destroy();
    //ENDIF
}
