#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InventoryComponent.h"
#include "MiniRPGPickup.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UPointLightComponent;

// A rotating, glowing cube pickup -- overlapping it with anything that has
// an InventoryComponent adds the configured item and removes the pickup.
UCLASS()
class AMiniRPGPickup : public AActor
{
	GENERATED_BODY()

public:
	AMiniRPGPickup();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Pickup")
	UPointLightComponent* GlowLight;

	UPROPERTY(EditAnywhere, Category = "Pickup")
	FString ItemName = TEXT("Health Potion");

	UPROPERTY(EditAnywhere, Category = "Pickup")
	EItemType PickupType = EItemType::Potion;

	UPROPERTY(EditAnywhere, Category = "Pickup")
	int32 ItemValue = 15;

	void ConfigurePickup(const FString& InName, EItemType InType, int32 InValue);

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void ApplyTint();
};
