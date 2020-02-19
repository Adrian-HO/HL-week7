// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"

UENUM(BlueprintType)
enum class EPickupMode : uint8
{
	RayCast,
	AutoPickup,
	PickupEquip//,
	//Default
};

UCLASS()
class GAME_API APickupActor : public AActor
{
	GENERATED_BODY()
#pragma region Region_Private

#pragma endregion Region_Private

#pragma region Region_Public
public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup Mode")
		EPickupMode PickupMode = EPickupMode::AutoPickup;


	// Sets default values for this actor's properties
	APickupActor();
	
	virtual void Tick(float DeltaTime) override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UFUNCTION(NetMulticast, Reliable)//, WithValidation)
	virtual void Disable();

	UFUNCTION(NetMulticast, Reliable)//, WithValidation)
	virtual void Enable();

#pragma endregion Region_Public

#pragma region Region_Protected
protected:
	class UPrimitiveComponent* Primitive;

    // Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
#pragma endregion Region_Protected
};
