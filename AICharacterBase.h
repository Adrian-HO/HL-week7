// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "AICharacterBase.generated.h"

/**
*
*/

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle,
	Suspicious,
	Alerted,
	Attack
};

UCLASS()
class GAME_API AAICharacterBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAICharacterBase();

protected:
	virtual void BeginPlay() override;

	//Add AIModule to Build.cs deps
	UPROPERTY(VisibleAnywhere, Category = "Components")
		UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()//Must be a UFUnc otherwise we cant bind this function
		void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
		void OnNoiseHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);

	FRotator OriginalRotator;

	UFUNCTION()
		void ResetOrientation();

	UFUNCTION()
		void ResetPatrol();

	FTimerHandle TimerHandle_ResetOrientation;
	FTimerHandle TimerHandle_ResetPatrol;

	//Net Code
	UPROPERTY(ReplicatedUsing = OnRep_GuardState)
		EAIState GuardState;
	UFUNCTION()
		void OnRep_GuardState(); //Gets called when GuardState changes, but only on the clients

	void SetGuardState(EAIState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
		void OnStateChanged(EAIState NewState);

	//Challenge - Adding Patrol
	UPROPERTY(EditInstanceOnly, Category = "AI") //Can set in editor for each instance. CAN BE CHANGED ONLY FROM BLUEPRINTS
		bool bPatrol;

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol")) //Will be enabled if bPatrol is true in the editor props
		AActor* FirstPatrolPoint;

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))//Will be enabled if bPatrol is true in the editor props
		AActor* SecondPatrolPoint;

	AActor* CurrentPatrolPoint;

	void MoveToNextPatrolPoint();


	TSubclassOf<class AWeaponBase> WeaponBlueprint;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    //TODO: Override ServerApplyDamage()
    //virtual void ServerApplyDamage_Implementation() override;
    //virtual bool ServerApplyDamage_Validate() override;
private:
	AActor* TargetActor;

};
