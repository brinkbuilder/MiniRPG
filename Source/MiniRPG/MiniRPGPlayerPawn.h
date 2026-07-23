#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MiniRPGPlayerPawn.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class URPGStatsComponent;
class UInventoryComponent;

// The player-controlled orb: floating movement, a follow camera, and
// combat/inventory bolted on via components shared with the enemy pawn.
UCLASS()
class AMiniRPGPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	AMiniRPGPlayerPawn();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UFloatingPawnMovement* MovementComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	URPGStatsComponent* StatsComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRange = 180.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackCooldown = 0.6f;

private:
	float LastAttackTime = -10.0f;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void OnAttackPressed();
	void OnUseItemPressed();
};
