// Copyright (c) 2017 GAM1528. All Rights Reserved.

#pragma once

#include <Components/ActorComponent.h>
#include "HealthComponent.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDeathSignature, AActor*, KilledBy);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAME_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	UPROPERTY(BlueprintReadOnly)
	float MaximumHealth;

    //TODO: Replicate this variable
	UPROPERTY(BlueprintReadOnly, Replicated)
	float CurrentHealth;

	UPROPERTY(BlueprintAssignable)
	FOnDeathSignature OnDeath;

	UFUNCTION()
	bool IsDead() const;

	//TEMPPPP
	//void TakeNetAIDamage(float Damage);
    void TakeNetAIDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

    void ReplenishHealth(int ammount);

protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

};
