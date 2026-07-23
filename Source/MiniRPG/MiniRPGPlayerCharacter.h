#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MiniRPGPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class URPGStatsComponent;
class UInventoryComponent;

// The player-controlled character: a real animated humanoid (engine
// tutorial mesh + anim blueprint) with a follow camera, plus
// combat/inventory bolted on via components shared with the enemy.
UCLASS()
class AMiniRPGPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMiniRPGPlayerCharacter();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	URPGStatsComponent* StatsComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRange = 220.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackCooldown = 0.6f;

private:
	float LastAttackTime = -10.0f;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void OnAttackPressed();
	void OnUseItemPressed();
};
