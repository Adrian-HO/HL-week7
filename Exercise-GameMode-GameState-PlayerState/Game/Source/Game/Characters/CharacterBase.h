// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include <GameFramework/Character.h>
#include "CharacterBase.generated.h"

UCLASS()
class GAME_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	float MaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly)
	float MaxJogSpeed;

	UPROPERTY(EditDefaultsOnly)
	float MaxPickUpDistance;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* FireHipAnimation;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* FireAimAnimation;

    //TODO Week 7: The player's name (usually loaded from a save file or database)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MPCharacter|Name", Replicated)
        FString PlayerName;

    //TODO Week 7:The name of the player that this character was last hit by.    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MPCharacter|Debug", Replicated)
        FName LastDamagedByName;

    //TODO Week 7:The player's current team.
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MPCharacter|Gameplay", Replicated)// ReplicatedUsing = OnRep_ChangeColor)//meta = (ClampMin = "1", ClampMax = "2"), Replicated
        int playerTeam;

    //TODO Week 7:Size of team one (grabbed from game state)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MPCharacter|Debug", Replicated) //meta = (EditCondition = "AreTeamsEnabled", ClampMin = "0")
        int TeamOneCount;

    //TODO Week 7: Size of team two (grabbed from game state)
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MPCharacter|Debug", Replicated) // , meta = (EditCondition = "AreTeamsEnabled", ClampMin = "0")
        int TeamTwoCount;

    //TODO Week 7:Team one's score (grabbed from game state)   
    UPROPERTY(BlueprintReadWrite, Category = "MPCharacter|Debug", Replicated)//, meta = (EditCondition = "AreTeamsEnabled", ClampMin = "0")
        int TeamOneScore;

    //TODO Week 7:Team two's score (grabbed from game state) 
    UPROPERTY(BlueprintReadWrite, Category = "MPCharacter|Debug", Replicated) //, meta = (EditCondition = "AreTeamsEnabled", ClampMin = "0")
        int TeamTwoScore;

    //TODO Week 7:Used to reference the player's current netID 
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "MPCharacter|Debug", Replicated)
        int NetIndex;

    //TODO Week 7: Players Material Color
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MPCharacter|Mesh", Replicated)
        TArray<class UMaterialInterface*> DefaultTPMaterials;

	UFUNCTION(BlueprintPure)
	class AWeaponBase* GetCurrentWeapon();

    //TODO: Call this function on the server
	UFUNCTION(Reliable, Server, WithValidation)
		virtual void ServerApplyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

    //TODO Week 7: Getters for PlayerState and GameState
    class ABasePlayerState* GetBasePlayerState();
    class ABaseGameState* GetGameState();


    //TODO Week 7: Assigns a team to the player that logs in
    void AssignTeams();
    //TODO Week 7: Assigns a Network Index to the player that logs in
    void AssignNetIndex();
    //TODO Week 7: Multicasts to all clients to assign the team color to Simulated_Proxy's and Autonomous_Proxy's when player logs in(CALLED FROM SERVER)
    UFUNCTION(NetMulticast, Reliable)
        void Multicast_AssignTeamsColor();

    //TODO Week 7: assign the team color to the mesh to Simulated_Proxy's and Autonomous_Proxy's when player logs in(CALLED FROM SERVER)
    UFUNCTION(BlueprintCallable, Category = FPSWizard)//
        void ApplyMaterialsToMesh(USkeletalMeshComponent* InMeshComp, const TArray<UMaterialInterface*>& InMaterials);

    //TODO Week 7: Sets the material to the mesh
    void SetMaterialToMesh(USkeletalMeshComponent* InMeshComp, const TArray<UMaterialInterface*>& InMaterials);

    //TODO Week 7: Respawn the player after death
    void Respawn();

public:
	void Move(FVector Direction, float Scale);
    //No need to make this Server, since weapon fires from the server
	void Fire(bool Toggle);

    //TODO: Call this function on the server
    UFUNCTION(Reliable, Server, WithValidation)
	void ServerAim(bool Toggle);

	void Interact(AActor* Actor);

	class USkeletalMeshComponent* GetSkeletalMesh();

	class UHealthComponent* GetHealth() { return Health; }

	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;


    //TODO Week 7: Called slightly after begin play, can be used to initialize the player after logging in
    UFUNCTION(BlueprintCallable, Category = FPSWizard)
        void PostBeginPlay();


protected:
	ACharacterBase();

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnWeaponFired();

	UFUNCTION()
	void OnDeath(AActor* KilledBy);

    //TODO: Call this function on the server, Net Multicast to all clients
	UFUNCTION(NetMulticast, Reliable, WithValidation)
		void NetMulticastOnDeath();

	//NET CODE 1 NET PICKUP
     //TODO: Call this function on the server,
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerHoldWeapon(class AWeaponBase* Weapon);

	//NET CODE 1 NET PICKUP
     //TODO: Call this function on the server,
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerDropWeapon();



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI Noise")
		UPawnNoiseEmitterComponent* NoiseEmitterComponent;
/////////////////////////////////////////////////////////////////////////
     //TODO: Replicate this variable on the server using the Movement Function,
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = Movement)
		FVector CurrentFacing;
	/** Change the facing of the character */
	void ChangeFacing(FVector TargetVector);

	/** Change the facing of the character on server */
      //TODO: Call this function on the server,
	UFUNCTION(Unreliable, Server, WithValidation)
		void ServerChangeFacing(FVector TargetVector);

	UPROPERTY(BlueprintReadOnly, Replicated) // UPROPERTY(BlueprintReadOnly)   
	bool bIsAiming;

    UPROPERTY(BlueprintReadOnly, Replicated)//UPROPERTY(BlueprintReadOnly)    
        bool bHasWeapon;
     
    UPROPERTY(BlueprintReadOnly, Replicated)//UPROPERTY(BlueprintReadOnly)
        bool bIsFiring;

    class UHealthComponent* Health;

///////////////////////////////////////////////////////////////////////
//NET CODE 1 NET PICKUP
//	UPROPERTY(ReplicatedUsing=OnRep_CurrentWeapon)
   //TODO: Replicate this variable to all clients
	UPROPERTY(Replicated)
		class AWeaponBase* CurrentWeapon;
	//NET CODE 1 NET PICKUP
//	UFUNCTION()
		void AttachCurrentWeapon();

		FVector CursorInput;
		
		void SetCursorXAxis(float AxisValue);
		void SetCursorYAxis(float AxisValue);


        //NEW
            //Called after BeginPlay, loads our savegame and gets/sets PlayerState info
        //TODO Week 7: Can be run in a loop to update the players stats and player states
        UFUNCTION(BlueprintCallable, Category = FPSWizard)
            virtual void UpdateAndCheckPlayer();

        //TODO Week 7: Update Player Timer
        //Calls UpdateAndCheckPlayer
        FTimerHandle UpdateHandle;

        //TODO Week 7: PostBeginPlay Timer
        UPROPERTY(Replicated)
        FTimerHandle PostBeginPlayDelay;

private:
    //TODO Week 7: New Capsule COmponent to detect Pickups
    UPROPERTY(VisibleAnywhere, Category = "Overlap")
    class UBoxComponent* OverlapComponent;

	class USkeletalMeshComponent* SkeletalMesh;
	class UCharacterBaseAnimation* AnimationInstance;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UDecalComponent* CursorToWorld;

    //TODO Week 7: Handle Overlap to detect pickups
    UFUNCTION()
    void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
};
