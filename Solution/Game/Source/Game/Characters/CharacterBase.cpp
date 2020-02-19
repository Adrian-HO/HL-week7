// Copyright (c) 2017 GAM1528. All Rights Reserved.

#include "Game.h"
#include "CharacterBase.h"
#include "CharacterBaseAnimation.h"

#include <GameFramework/PlayerController.h>
#include "Gameplay/Health/HealthComponent.h"
#include "Weapons/WeaponBase.h"
#include "Items/ItemBase.h"
#include "Gameplay/BasePlayerState.h"
#include "Gameplay/BaseGameState.h"
#include "Gameplay/BaseGameMode.h"

#include "Pickups/HealthPickup.h"
#include "Pickups/AmmoPickup.h"

//TODO: Include this header
#include "Net/UnrealNetwork.h"
//#include "OnlineGameInstance.h"
#include "Components/BoxComponent.h"

/*
if (Role == ROLE_Authority) ; // Will always mean server version.
if (Role == ROLE_AutonomousProxy) ; // Will always mean the client that 'owns' the actor.
if (Role == ROLE_SimulatedProxy) ; // Will always mean the client(s) that are just copying the actors data from the server.
*/


ACharacterBase::ACharacterBase() :
	Health(nullptr),
	SkeletalMesh(nullptr),
	AnimationInstance(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	// Set default movement speed.
	MaxWalkSpeed = 140.0f;
	MaxJogSpeed = 280.0f;

	GetCharacterMovement()->MaxWalkSpeed = MaxJogSpeed;

	// Do not update the controller's rotation yaw.
	// Has to be disabled for "Orient Rotation to Movement" to work.
	bUseControllerRotationYaw = false;

	//bReplicates = true;
	//bReplicateMovement = true;
    bAlwaysRelevant = true;
	SetReplicates(true);
	SetReplicateMovement(true);

	// Rotate the character during movement by default.
	GetCharacterMovement()->bOrientRotationToMovement = true;

	GetCharacterMovement()->SetIsReplicated(true);
	GetCharacterMovement()->SetNetAddressable();
	// Character interaction values.
	MaxPickUpDistance = 200.0f;

	// Character weapon handling values.
	CurrentWeapon = nullptr;

	bIsAiming = false;
	bIsFiring = false;

	SetCanBeDamaged(true);

	NoiseEmitterComponent = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("Noise Emitter"));

    //TODO Week 7: Setup the Overlap Component
    OverlapComponent = CreateDefaultSubobject<UBoxComponent>("OverlapSphere");
    OverlapComponent->SetupAttachment(RootComponent);
    OverlapComponent->SetGenerateOverlapEvents(true);
    OverlapComponent->bDynamicObstacle = false;
    //SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    OverlapComponent->BodyInstance.SetCollisionProfileName("Trigger");//OverlapAllDynamic
    OverlapComponent->CanCharacterStepUpOn = ECB_No;
    OverlapComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    OverlapComponent->SetNotifyRigidBodyCollision(false);
    OverlapComponent->OnComponentBeginOverlap.AddDynamic(this, &ACharacterBase::HandleOverlap);

    //TODO Week 7: Create the cursor - This is taken from ther TopDown template
	// Create a decal in the world to show the cursor's location
    CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
    CursorToWorld->SetupAttachment(RootComponent);
    static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
    if (DecalMaterialAsset.Succeeded())
    {
        CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
    }
    CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
    CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());



    Tags.Empty();
    Tags.Add("Character");
}

void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Retrieve the health component.
	Health = FindComponentByClass<UHealthComponent>();
	//check(Health != nullptr && "Character does not have a health component!");

	// Subscribe to health component's events.
	if(Health != nullptr)
		Health->OnDeath.AddDynamic(this, &ACharacterBase::OnDeath);

	// Retrieve the skeletal mesh component.
	SkeletalMesh = GetMesh();
	if (SkeletalMesh != nullptr)
	{
  	    //check(SkeletalMesh != nullptr && "Character does not have a skeletal mesh component!");

		//Retrieve the animation instance.
		AnimationInstance = Cast<UCharacterBaseAnimation>(SkeletalMesh->GetAnimInstance());
		//check(AnimationInstance != nullptr && "Character does not have an animation instance!");
	}


}

void ACharacterBase::SetupPlayerInputComponent(class UInputComponent* pInputComponent)
{
	Super::SetupPlayerInputComponent(pInputComponent);

    //TODO Week 7: Set the player name
    //IF GetPlayerState()
    if(GetBasePlayerState())
        //CALL GetPlayerState()->SetPlayerName() and pass in PlayerName
        GetBasePlayerState()->SetPlayerName(PlayerName);
    //ENDIF

       
}
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
    
    PlayerName = GetFName().ToString();//TEMP
  
    //TODO Week 7: Attempt to set the NetIndex
    if (GetLocalRole() > ROLE_AutonomousProxy)
    {
        //WHOOPS! WON'T WORK IF PLAYERS ARE ALL ADDED AT SAME TIME
        //Set an index reference for this character based on what number player it is in the game
        //SET/ASSIGN NetIndex to GetWorld()->GetGameState()->AuthorityGameMode->GetNumPlayers() - 1
        NetIndex = GetWorld()->GetGameState()->AuthorityGameMode->GetNumPlayers() - 1;
    }

    //TODO Week 7: START the UpdateHandle and PostBeginPlay Timers
    //UpdateHandle arguments (looping): UpdateHandle, this, &ACharacterBase::UpdateAndCheckPlayer, 0.03333f, true, 0.0f 
    GetWorldTimerManager().SetTimer(UpdateHandle, this, &ACharacterBase::UpdateAndCheckPlayer, 0.03333f, true, 0.0f);
    //PostBeginPlay arguments (non looping: PostBeginPlayDelay, this, &ACharacterBase::PostBeginPlay, 1.0f, false
    GetWorldTimerManager().SetTimer(PostBeginPlayDelay, this, &ACharacterBase::PostBeginPlay, 2.0f, false);
    
}
void ACharacterBase::PostBeginPlay()
{
    //TODO Week 7: Assign the AI character a team of -1
    if (ActorHasTag("AICharacter"))
    {
        playerTeam = -1;
        return;
    }

  //TODO Week 7: Assign Team Colors
    //IF Role is Authority
    if(GetLocalRole() == ROLE_Authority)
     //CALL Multicast_AssignTeamsColor()
     Multicast_AssignTeamsColor();  
    //ENDIF
}

void ACharacterBase::UpdateAndCheckPlayer()
{
    //TODO Week 7: Update variables from Player State
    if(GetGameState())
    {
        TeamOneCount = GetGameState()->TeamOneSize;
        TeamTwoCount = GetGameState()->TeamTwoSize;
        TeamOneScore = GetGameState()->TeamOneScore;
        TeamTwoScore = GetGameState()->TeamTwoScore;
    }

}

//TODO Week 7: Assign the team
//Auto-assigns the player to team one or two so that they are balanced in numbers
void ACharacterBase::AssignTeams()
{
    if (!GetGameState())
        return;

    if (ActorHasTag("AICharacter"))
    {
        playerTeam = -1;
        return;
    }
    TeamOneCount = GetGameState()->TeamOneSize;
    TeamTwoCount = GetGameState()->TeamTwoSize;

    //IF TeamOneCOunt is EQUAL to TeamTwoCount
    if (TeamOneCount == TeamTwoCount)
    {
        //IF The Net Mode is Dedicated Server
        if (GetNetMode() == ENetMode::NM_DedicatedServer)
        {
            //GET The GameState and Increment TeamOneSize
            GetGameState()->TeamOneSize++;
            //SET/ASSIGN playerTeam to 0
            playerTeam = 0;
        }
        else
        {
            //GET The GameState and Increment TeamTwoSize
            GetGameState()->TeamTwoSize++;
            //SET/ASSIGN playerTeam to 1
            playerTeam = 1;
        }
    }
    //ENDIF
    //If either of the teams is larger than the other, assign the player to the team with less to balance them
    //ELSE
    else
    {
        //IF TeamOneCount is GREATER than TeamTwoCount
        if (TeamOneCount > TeamTwoCount)
        {
            //GET the Game State and Increment TeamTwoSize
            GetGameState()->TeamTwoSize++;
            //SET/Assign playerTeam to 1
            playerTeam = 1;
        }
        else if (TeamOneCount < TeamTwoCount)
        {
            //GET the Game State and Increment TeamOneSize
            GetGameState()->TeamOneSize++;
            //SET/Assign playerTeam to 0
            playerTeam = 0;
        }
    }//ENDELSE

    //Update the PlayerTeam on the PlayerState
    //CALL GetPlayerState() and SET the PlayerTeam to this instance's playerTeam
    if (GetBasePlayerState())
        GetBasePlayerState()->PlayerTeam = playerTeam;
}

//TODO Week 7: Sets the Net Index for this character
void ACharacterBase::AssignNetIndex()
{
    NetIndex = GetGameState()->AuthorityGameMode->GetNumPlayers() - 1;
}

//TODO Week 7:Sets the player's 3rd person materials to their respective team's materials if specified in game state
void ACharacterBase::Multicast_AssignTeamsColor_Implementation()
{ 
    //IF GetGameSate()
    if (GetGameState()) //
    {       
        //If we're on team one
        //IF playerTEam is 0
        if (playerTeam == 0)
        {
            //If the first person material array for team one isn't null,
            //assign those materials to the first person mesh
            //IF GetGameState()->TeamOnePMaterials.Num() is GREATER than 0
            if (GetGameState()->TeamOnePMaterials.Num() > 0)
            {
                //SET/ASSIGN DefaultTPMaterials to the GameStates's TeamOnePMaterials
                DefaultTPMaterials = GetGameState()->TeamOnePMaterials;
                //CALL ApplyMaterialsToMesh() and pass in GetSkeletalMesh(), DefaultTPMaterials
                ApplyMaterialsToMesh(GetSkeletalMesh(), DefaultTPMaterials);
            }

        }
        //ELSE IF playerTeam is 1  //Otherwise if we're on team two, do the same as above but for team two        
        else if (playerTeam == 1)
        {
            //IF GetGameState()->TeamTwoPMaterials.Num() is GREATER than 0
             if (GetGameState()->TeamTwoPMaterials.Num() > 0)
             {
                 //SET/ASSIGN DefaultTPMaterials to the GameStates's TeamTwoPMaterials
                 DefaultTPMaterials = GetGameState()->TeamTwoPMaterials;
                 //CALL ApplyMaterialsToMesh() and pass in GetSkeletalMesh(), DefaultTPMaterials
                 ApplyMaterialsToMesh(GetSkeletalMesh(), DefaultTPMaterials);
             }
        }
        //ENDIF
    }
    //ENDIF
}

//TODO Week 7: Assigns an array of materials to a SkeletalMeshComponent via Multicast _Implementation
void ACharacterBase::ApplyMaterialsToMesh(USkeletalMeshComponent* InMeshComp, const TArray<UMaterialInterface*>& InMaterials)
{
    //CALL SetMaterialToMesh() pass in InMeshComp, InMaterials
    SetMaterialToMesh(InMeshComp, InMaterials);
}
//TODO Week 7: Set the material to the mesh passed in
void ACharacterBase::SetMaterialToMesh(USkeletalMeshComponent* InMeshComp, const TArray<UMaterialInterface*>& InMaterials)
{
    //If our specified SkeletalMeshComponent isn't null and our array isn't empty
    //IF InMeshComp is NOT nullptr AND InMaterials.Num() IS GREATER THAN 0
    if (InMeshComp != nullptr && InMaterials.Num() > 0)
    {
        //For each material in the array (InMaterials), assign it to the respective slot in the SkeletalMeshComponent (Can use a regular For Loop)
        for (int i = 0; i < InMaterials.Num(); i++)
        {
            //GET the material at "i" in InMaterials and assign it to a temporary variable called MaterialToAssign of type UMaterialInterface*
            UMaterialInterface* MaterialToAssign = InMaterials[i];
            //IF MaterialToAssign is NOT nullptr
            if (MaterialToAssign != nullptr)
            {
                //CALL SetMaterial() on the InMeshComp and pass in i, MaterialToAssign
                InMeshComp->SetMaterial(i, MaterialToAssign);
            }
            //ENDIF
        }
        //ENDFOR
    }
    //ENDIF
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    //TODO Week 7: Display the NetIndex and player team
    if (GetBasePlayerState())
    {
        UKismetSystemLibrary::DrawDebugString(this, FVector(0.f, 0.f, -10.f), "NetIndex - " + FString::FromInt(NetIndex) + " : Team - " + FString::FromInt(GetBasePlayerState()->PlayerTeam), this, FLinearColor::Black);
    }
	//Cursor
    if (CursorToWorld != nullptr)
    {
        if (APlayerController* PC = Cast<APlayerController>(GetController()))
        {
            if (PC->IsLocalPlayerController())
            {
                FHitResult TraceHitResult;
                PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
                FVector CursorFV = TraceHitResult.ImpactNormal;
                FRotator CursorR = CursorFV.Rotation();
                CursorToWorld->SetWorldLocation(TraceHitResult.Location);
                CursorToWorld->SetWorldRotation(CursorR);
            }

        }
    }

	// Check if we have a weapon.
	//const bool bHasWeapon = CurrentWeapon != nullptr;

	bHasWeapon = CurrentWeapon != nullptr; //LAST

	// Handle movement orientation and speed.
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = MaxJogSpeed;

	if(bHasWeapon && (bIsAiming || bIsFiring))
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;

		if(bIsAiming)
		{
			GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
		}

		if(bIsFiring)
		{
			GetCharacterMovement()->MaxWalkSpeed *= 0.8;
		}
	}

	// Rotate the character towards the aiming point.
	if(bHasWeapon && (bIsAiming || bIsFiring))
	{
		auto PlayerController = Cast<APlayerController>(GetController());

		if(PlayerController)
		{
			// Cast cursor trace to world.
			FVector MouseLocation;
			FVector MouseDirection;

			bool Success = PlayerController->DeprojectMousePositionToWorld(MouseLocation, MouseDirection);

			// Rotate the character towards the cursor.
			if(Success)
			{
				// Cast the cursor onto a plane.
				FVector LineBegin = MouseLocation;
				FVector LineEnd = MouseLocation + MouseDirection * 10000.0f;

				FVector PlaneOrigin = CurrentWeapon->GetMuzzleLocation();
				FVector PlaneNormal = FVector(0.0f, 0.0f, 1.0f);

				FVector LookLocation = FMath::LinePlaneIntersection(LineBegin, LineEnd, PlaneOrigin, PlaneNormal);

				// Rotate the cursor toward the intersection of the cursor and the plane.
				FRotator LookRotation = (LookLocation - GetActorLocation()).Rotation();

				LookRotation.Pitch = 0.0f;
				LookRotation.Roll = 0.0f;
				FRotator rot = FMath::RInterpTo(GetActorRotation(), LookRotation, DeltaTime, 10.0f);
				
                //TODO: RPC the rotation
                //CALL ChangeFacing() on the server
				ChangeFacing(rot.Vector()); //FMath::RInterpTo(GetActorRotation(), LookRotation, DeltaTime, 10.0f).Vector()
			}
		}
	}

	// Set animation weapon parameters.
	AnimationInstance->bHasWeapon = bHasWeapon;
	AnimationInstance->bIsAiming = bIsAiming;

	// Set animation movement parameters.
	float CurrentSpeed = GetVelocity().Size();
	bool bIsMoving = CurrentSpeed > 0.0f && GetCharacterMovement()->IsMovingOnGround();

	AnimationInstance->bIsMoving = bIsMoving;
	AnimationInstance->MovementSpeed = bIsMoving ? CurrentSpeed : 0.0f;

	// Set animation strafing rotation paremeter.
	FVector MovementDirection = GetLastMovementInputVector();
	FVector CharacterDirection = GetActorForwardVector();

	if(!MovementDirection.IsNearlyZero())
	{
		float StrafingRotation = FMath::Atan2(MovementDirection.Y, MovementDirection.X) - FMath::Atan2(CharacterDirection.Y, CharacterDirection.X);

		if(FMath::Abs(StrafingRotation) > PI)
		{
			StrafingRotation = StrafingRotation > 0 ? StrafingRotation - PI * 2.0f : StrafingRotation + PI * 2.0f;
		}

		StrafingRotation = FMath::RadiansToDegrees(StrafingRotation);

		AnimationInstance->StrafingRotation = StrafingRotation;
	}

}


void ACharacterBase::ChangeFacing(FVector TargetVector)
{
	// We set the characters facing to match the facing of the vector
    //CALL SetActorRotation() and pass in TargetVector.Rotation()
	SetActorRotation(TargetVector.Rotation());

	// We set our current facing to the direction the actor is facing
    //SET/ASSIGN CurrentFAcing to TargetVector
	CurrentFacing = TargetVector;
    //IF Role is LESS THAN ROLE_Authority
	if (GetLocalRole() < ROLE_Authority)
	{
        //CALL ServerChangeFacing() and pass in CurrentFacing
		ServerChangeFacing(CurrentFacing);
	}
    //ENDIF
}


void ACharacterBase::ServerApplyDamage_Implementation(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("ACharacterBase:: DamageCaused"));
    //TODO Week 7: SET the last damaged by on the PlayerState
    //IF (GetPlayerState()
    if (GetBasePlayerState())
    {
        //SET the LastDamagedByName on the player state (CALL GetPlayerState) to the DamageCauser's PlayerName (Cast DamageCauser as ACharacterBase)
        GetBasePlayerState()->LastDamagedByName = Cast<ACharacterBase>(DamageCauser)->PlayerName;
        //DRAW a debug Message
        GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Red,"Damaged By" + GetBasePlayerState()->LastDamagedByName);
        
    }
    //ENDIF
    FDamageEvent DamageEvent;
	TakeDamage(Damage, DamageEvent, InstigatedBy, DamageCauser);//Does not fire for AI in Dedicated Server
}

bool ACharacterBase::ServerApplyDamage_Validate(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	return true;
}

//void ACharacterBase::ApplyDamageAI_Implementation()
//{
//    UE_LOG(LogTemp, Warning, TEXT("ACharacterBase:: DamageCaused"));
//
//    FDamageEvent DamageEvent;
//    Health->TakeNetAIDamage(10.0f);//This is a work around for now until network owner is figured out in Dedicated Server mode
//    Super::APawn::TakeDamage(10.0f, DamageEvent, nullptr, this);//FIgure out how to make this fire. The AI Object Already in the scene does not have a Owner.
//}
//
//bool ACharacterBase::ApplyDamageAI_Validate()
//{
//    return true;
//}

bool ACharacterBase::ServerChangeFacing_Validate(FVector TargetVector) {
	return true;
}
void ACharacterBase::ServerChangeFacing_Implementation(FVector TargetVector) {
	ChangeFacing(TargetVector);
}



void ACharacterBase::Move(FVector Direction, float Scale)
{
	AddMovementInput(Direction, Scale);
}

void ACharacterBase::Fire(bool Toggle)
{
	if(CurrentWeapon)
	{
		if(Toggle)
		{
			CurrentWeapon->PullTrigger();
		}
		else
		{
			CurrentWeapon->ReleaseTrigger();
		}
		
		bIsFiring = Toggle;
	}
}

void ACharacterBase::ServerAim_Implementation(bool Toggle)
{
	bIsAiming = Toggle;
}

bool ACharacterBase::ServerAim_Validate(bool Toggle)
{
    return true;
}

void ACharacterBase::Interact(AActor* Actor)
{

	UE_LOG(LogClass, Log, TEXT("%s pawn begin play: %s .. ROLE on machine: %s / REMOTE ROLE: %s  "),
		(IsLocallyControlled() ? TEXT("LOCAL") : TEXT("proxy")),
		*GetName(),
		*UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetLocalRole()),
		*UEnum::GetValueAsString(TEXT("Engine.ENetRole"), GetRemoteRole()));

	// Drop the current weapon.
	ServerDropWeapon();
	// Check if the actor is an item.
	AItemBase* Item = Cast<AItemBase>(Actor);

	if(Item == nullptr)
		return;

	// Check distance from the object.
	float Distance = Item->GetDistanceTo(this);

	if(Distance > MaxPickUpDistance)
		return;

	// Hold the item if it's a weapon.
	AWeaponBase* Weapon = Cast<AWeaponBase>(Item);

	if(Weapon != nullptr)
	{
		ServerHoldWeapon(Weapon);
	}
}
//NET CODE 1 NET PICKUP
//TODO: HoldWeapon Validate Function
bool ACharacterBase::ServerHoldWeapon_Validate(AWeaponBase* Weapon)
{
	return true;
}
//NET CODE 1 NET PICKUP
//TODO: HoldWeapon Implementation Function
void ACharacterBase::ServerHoldWeapon_Implementation(AWeaponBase* Weapon)
{
	check(Weapon != nullptr && "Passed a null weapon!");

	// Drop currently carried weapon first.
    //CALL ServerDropWeapon()
	ServerDropWeapon();

	// Attach weapon to the character.
    //IF CurrentWeapon is nullptr
	if (CurrentWeapon == nullptr)
	{
		//NET CODE 3
        //IF the ROle is Role_Authority
		if (GetLocalRole() == ROLE_Authority)
		{
            //SET/ASSIGN CurrentWeapon to Weapon
			CurrentWeapon = Weapon;
            //CALL AttachCurrentWeapon
			AttachCurrentWeapon();
		}
        //ENDIF
	}
    //ENDIF
}
//NET CODE 1 NET PICKUP
void ACharacterBase::AttachCurrentWeapon()
{
    //IF CurrentWeapon is NOT nullptr
	if (CurrentWeapon != nullptr)
	{
        //CALL Attach() on the CUrrentWeapon and pass in this
		CurrentWeapon->Attach(this);
        //CALL Clear() on the CurrentWeapon's OnWeaponFired Event/Delegate
		CurrentWeapon->OnWeaponFired.Clear();
		// Subscribe to weapon's events.
        //SUBSCRIBE to the CUrrentWeapon's OnWeaponFired Event/Delegate
		CurrentWeapon->OnWeaponFired.AddDynamic(this, &ACharacterBase::OnWeaponFired);
	}
    //ENDIF
}

void ACharacterBase::SetCursorXAxis(float AxisValue)
{
	CursorInput.X = AxisValue;
	UE_LOG(LogTemp, Warning, TEXT("%f"), CursorInput.X);
	//GetController()->SetControlRotation(UGameplayStatics::R)
	FRotator MovementRotation = CursorInput.Rotation();
	SetActorRotation(MovementRotation);
}

void ACharacterBase::SetCursorYAxis(float AxisValue)
{
	CursorInput.Y = AxisValue;
	UE_LOG(LogTemp, Warning, TEXT("%f"), CursorInput.Y);
	FRotator MovementRotation = CursorInput.Rotation();
	SetActorRotation(MovementRotation);
}



//NET CODE 1 NET PICKUP
//TODO: DropWeapon Validate Function
bool ACharacterBase::ServerDropWeapon_Validate()
{
	return true;
}
//NET CODE 1 NET PICKUP
//TODO: DropWeapon Implementation Function
void ACharacterBase::ServerDropWeapon_Implementation()
{
	if(CurrentWeapon != nullptr)
	{
		// Unsubscribe from weapon's events.
		CurrentWeapon->OnWeaponFired.RemoveDynamic(this, &ACharacterBase::OnWeaponFired);
		CurrentWeapon->OnWeaponFired.Clear();
		// Detach weapon from the character.
		CurrentWeapon->Detach();
		CurrentWeapon = nullptr;

		// Reset weapon states states.
		bIsFiring = false;
		bIsAiming = false;
	}
}

void ACharacterBase::OnWeaponFired()
{
	// Play recail animation depending on the stance.
	if(bIsAiming)
	{
		AnimationInstance->Montage_Play(FireAimAnimation);
	}
	else
	{
		AnimationInstance->Montage_Play(FireHipAnimation);
	}
}
//TODO: OnClientDeath Implementation Function
void ACharacterBase::NetMulticastOnDeath_Implementation()
{
	// Disable character's capsule collision.
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Allow character's ragdoll to be pushed around.
	/*
	SkeletalMesh->SetCollisionProfileName(TEXT("BlockAll"));
	SkeletalMesh->CanCharacterStepUpOn = ECB_No;
	SkeletalMesh->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	*/
	UE_LOG(LogTemp, Warning, TEXT("DEADED"));
	// Simulate character's ragdoll.
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SkeletalMesh->SetSimulatePhysics(true);
}
//TODO: OnClientDeath Validate Function
bool ACharacterBase::NetMulticastOnDeath_Validate()
{
	return true;
}

//Will be fired on Server
void ACharacterBase::OnDeath(AActor* KilledBy)
{
    //TODO Week 7: Set Last Damaged By when Killed
    //If the PlayerState is not null (Call GetPlayerState())
    if(GetBasePlayerState())
    {
        //SET the LastDamagedByName on the player state (CALL GetPlayerState) to the KilledBy's PlayerName (Cast KilledBy as ACharacterBase)
        GetBasePlayerState()->LastDamagedByName = Cast<ACharacterBase>(KilledBy)->PlayerName;
        //DRAW a debug message
        GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Black, "Killed By: " + GetBasePlayerState()->LastDamagedByName);
    }
    //ENDIF	

	// Stop ticking while dead.
	PrimaryActorTick.bCanEverTick = false;

	// Replicated Drop held weapon. // Fired On Client, Tell Server
	ServerDropWeapon();

	NetMulticastOnDeath(); 

    //TODO Week 7: Set the GameStae variables
    //IF Role is Authority
    if (GetLocalRole() == ROLE_Authority)
    {
        //IF playerTeam is 0
        if (playerTeam == 0)
        {
            //GetGameState and Increment TeamTwoScore
            GetGameState()->TeamTwoScore++;
            //GetGameState and decrement TeamOneSize
            GetGameState()->TeamOneSize--;
        }
        //ELSE
        else
        {
            //GetGameState and Increment TeamOneScore
            GetGameState()->TeamOneScore++;
            //GetGameState and decrement TeamTwoSize
            GetGameState()->TeamTwoSize--;
        }
        //ENDIF

        //START a respawn handler to CALL Respawn() after 5 seconds (Non Looping)
        FTimerHandle respawn;
        GetWorldTimerManager().SetTimer(respawn, this, &ACharacterBase::Respawn, 5.0f, false);
    }
    //ENDIF
   
}
//TODO Week 7: RESPAWN the Player
void ACharacterBase::Respawn()
{
    //IF Role is Authority
    if (GetLocalRole() == ROLE_Authority)
    {
        //GET the ABaseGameMode and assign it to a variable called GM
        ABaseGameMode* GM = Cast<ABaseGameMode>( GetWorld()->GetAuthGameMode() );
        //IF GM is NOT nullptr
        if (GM)
        {
            //CALL RespawnPlayer() on the GM passing in playerTeam, NetIndex
            GM->RespawnPlayer(Cast<APlayerController>( GetController() ), playerTeam, NetIndex);
            Destroy();
        }
        //ENDIF
    }
    //ENDIF
}


//TODO Week 7: RETURN the ABasePlayerState
ABasePlayerState* ACharacterBase::GetBasePlayerState()
{
    return Cast<class ABasePlayerState>(GetPlayerState());
}

//TODO Week 7: Return the ABaseGameState
class ABaseGameState* ACharacterBase::GetGameState()
{
    return Cast<ABaseGameState>(GetWorld()->GetGameState());
}



AWeaponBase* ACharacterBase::GetCurrentWeapon()
{
	return CurrentWeapon;
}

class USkeletalMeshComponent* ACharacterBase::GetSkeletalMesh()
{
	return SkeletalMesh;
}

//TODO Week 7: Replenish Health and/or Ammo
void ACharacterBase::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    GEngine->AddOnScreenDebugMessage(-1, 15, FColor::Black, "OVERLAPP");
    //IF Role is Authority
    if (GetLocalRole() == ROLE_Authority)// && IsLocallyControlled())
    {
        //IF OtherActor is not nullptr
        if (OtherActor)
        {
            //IF OtherActor has the tag "Health"
            if (OtherActor->ActorHasTag("Health"))
            {
                //DECLARE a AHealthPickup* called HealthPickup and ASSIGN it to the return of Cast<AHealthPickup>(OtherActor)
                AHealthPickup* HealthPickup = Cast<AHealthPickup>(OtherActor);
                //IF Health is NOT nullptr
                if (Health)
                {      
                    //CALL ReplenishHealth on Health (this actors Health component) and pass in the HealthPickup's HealthValue
                    Health->ReplenishHealth(HealthPickup->HealthValue);
                    //CALL Destroy on the HealthPickup
                    HealthPickup->Destroy();
                }
                //ENDIF
            }
            //ENDIF

             //IF OtherActor has the tag "Ammo"
            if (OtherActor->ActorHasTag("Ammo"))
            {
                //DECLARE a AAmmoPickup* called AmmoPickup and ASSIGN it to the return of Cast<AAmmoPickup>(OtherActor)
                AAmmoPickup* AmmoPickup = Cast<AAmmoPickup>(OtherActor);
                //IF CurrentWeapon is NOT nullptr
                if (CurrentWeapon)
                {
                    //CALL ReplenishAmmo() on the CurrentWeapon and pass in AmmoPickup's Capacity
                    CurrentWeapon->ReplenishAmmo(AmmoPickup->Capacity);
                    //CALL Destroy on the AmmoPickup
                    AmmoPickup->Destroy();
                }
                //ENDIF
            }
        }
        //ENDIF
    }
    //ENDIF
}


//TODO: GetLifetimeReplicatedProps(...)
void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//Don't want to replicate to client that owns this weapon
	//DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
	DOREPLIFETIME(ACharacterBase, CurrentWeapon);
	DOREPLIFETIME(ACharacterBase, CurrentFacing);

	DOREPLIFETIME(ACharacterBase, bIsAiming);
	DOREPLIFETIME(ACharacterBase, bIsFiring);
	DOREPLIFETIME(ACharacterBase, bHasWeapon);

    //TODO Week 7: Replicate vars
    DOREPLIFETIME(ACharacterBase, PlayerName);
    DOREPLIFETIME(ACharacterBase, LastDamagedByName);
    DOREPLIFETIME(ACharacterBase, playerTeam); 
    DOREPLIFETIME(ACharacterBase, NetIndex);
    

    // DOREPLIFETIME(ACharacterBase, TeamOneCount);
    // DOREPLIFETIME(ACharacterBase, TeamTwoCount);
    // DOREPLIFETIME(ACharacterBase, TeamOneScore);
    // DOREPLIFETIME(ACharacterBase, TeamTwoScore);
    DOREPLIFETIME(ACharacterBase, PostBeginPlayDelay);

    DOREPLIFETIME(ACharacterBase, DefaultTPMaterials);
    

}

