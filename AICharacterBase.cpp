// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "AICharacterBase.h"
#include "NavigationSystem.h"
#include "AIModule/Classes/Blueprint/AIBlueprintHelperLibrary.h"
#include "Net/UnrealNetwork.h" //needed for DOREPLIFETIME
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/WeaponBase.h"
#include "Gameplay/Health/HealthComponent.h"

AAICharacterBase::AAICharacterBase() : Super()
{
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AAICharacterBase::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AAICharacterBase::OnNoiseHeard);

	GuardState = EAIState::Idle;

	static ConstructorHelpers::FClassFinder<AWeaponBase> ItemBlueprint(TEXT("/Game/Weapons/Base/Blueprints/BP_WeaponBase"));
	if (ItemBlueprint.Class) {
		WeaponBlueprint = ItemBlueprint.Class;
	}
}

void AAICharacterBase::BeginPlay()
{
	Super::BeginPlay();

	OriginalRotator = GetActorRotation();

	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}
	if (Role == ROLE_Authority)
	{
		// Spawn a projectile.
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn; //TILAN HERE AdjustIfPossibleButAlwaysSpawn

		if (WeaponBlueprint) {
			//FTransform Transform = Muzzle->GetComponentToWorld();
			AWeaponBase* weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponBlueprint, SpawnParams);
			ServerHoldWeapon(weapon);
		}
	}
}

void AAICharacterBase::OnPawnSeen(APawn* SeenPawn)
{
	if (GetHealth() != nullptr && GetHealth()->IsDead())
	{
		ResetPatrol();
		return;
	}

	if (SeenPawn == nullptr)
	{
		return;
	}
	TargetActor = SeenPawn;
	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f);
	UE_LOG(LogTemp, Warning, TEXT("Seen"));
	
	SetGuardState(EAIState::Alerted);

	CurrentWeapon->PullTrigger();

	//Stop Movement if Patrolling
	AController* Controller = GetController();
	if (Controller)
	{

		Controller->StopMovement();
		GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
		GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AAICharacterBase::ResetPatrol, 3.0f);
	}
}

void AAICharacterBase::OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Green, false, 10.0f);
	UE_LOG(LogTemp, Warning, TEXT("heard"));

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

    FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator(); //Can use the one in kismet library (LookAt()) X is to look forward dir
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;
	SetActorRotation(NewLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AAICharacterBase::ResetOrientation, 3.0f);


	SetGuardState(EAIState::Suspicious);

	//Stop Movement if Patrolling
	AController* Controller = GetController();
	if (Controller)
	{
		Controller->StopMovement();
	}
}

void AAICharacterBase::ResetOrientation()
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}
	SetActorRotation(OriginalRotator);
	ResetPatrol();
}

void AAICharacterBase::ResetPatrol()
{
	TargetActor = nullptr;
	SetGuardState(EAIState::Idle);
	if(CurrentWeapon)
		CurrentWeapon->ReleaseTrigger();

	//Stop Investigating ... if we are a patrolling pawn. Pick a new Patrol point to move to
	if (bPatrol)
	{
		MoveToNextPatrolPoint();
	}
}

void AAICharacterBase::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

void AAICharacterBase::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState)
	{
		return;
	}
	GuardState = NewState;
	OnRep_GuardState();
}

void AAICharacterBase::MoveToNextPatrolPoint()
{

	if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == SecondPatrolPoint)
	{
		CurrentPatrolPoint = FirstPatrolPoint;
	}
	else
	{
		CurrentPatrolPoint = SecondPatrolPoint;
	}
    UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), CurrentPatrolPoint);
}

void AAICharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//Patrol Goal Checks
	if (CurrentPatrolPoint)
	{
		FVector Delta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();
		float DistanceToGoal = Delta.Size();
		//UE_LOG(LogTemp, Warning, TEXT("DistanceToGoal = %f"), DistanceToGoal);
		//Check if we are within 50 units of our goal. If so, pick a new patrol point
		if (DistanceToGoal < 150)
		{
			MoveToNextPatrolPoint();
		}
	}

	if (TargetActor)
	{
		FVector MyLoc = GetActorLocation();
		FVector TargetLoc = TargetActor->GetActorLocation();
		FVector Dir = (TargetLoc - MyLoc);
		Dir.Normalize();
		SetActorRotation(FMath::Lerp(GetActorRotation(), Dir.Rotation(), 0.05f));
	}
}


//void AAICharacterBase::ServerApplyDamage_Implementation()
//{
//    ApplyDamageAI();
//}
//
//bool AAICharacterBase::ServerApplyDamage_Validate()
//{
//    return true;
//}

//void AAICharacterBase::ApplyDamageAI_Implementation()
//{
//    UE_LOG(LogTemp, Warning, TEXT("AAICharacterBase:: DamageCaused"));
//
//    FDamageEvent DamageEvent;
//}
//
//bool AAICharacterBase::ApplyDamageAI_Validate()
//{
//    return true;
//}

void AAICharacterBase::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAICharacterBase, GuardState);

}

