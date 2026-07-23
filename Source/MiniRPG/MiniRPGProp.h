#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MiniRPGProp.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EGatherResource : uint8
{
	None,
	Wood,
	Ore
};

// A simple decorative actor (tree or rock) assembled from engine basic
// shapes at runtime -- purely visual set dressing by default, but trees
// yield Wood and rocks yield Ore when gathered (see AMiniRPGPlayerCharacter).
UCLASS()
class AMiniRPGProp : public AActor
{
	GENERATED_BODY()

public:
	AMiniRPGProp();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	UStaticMeshComponent* TopMesh;

	void ConfigureAsTree(float TrunkHeight, const FLinearColor& TrunkColor, const FLinearColor& LeafColor);
	void ConfigureAsRock(float Scale, const FLinearColor& Color);

	EGatherResource GetGatherResource() const { return GatherResource; }
	bool IsGatherable() const { return GatherResource != EGatherResource::None && !bDepleted; }

	// Rocks go dark and stop being gatherable for RespawnSeconds; trees
	// never deplete (matches ordinary RuneScape trees vs. ore rocks).
	void Deplete(float RespawnSeconds);

private:
	UPROPERTY()
	UStaticMesh* CubeMesh;

	UPROPERTY()
	UStaticMesh* ConeMesh;

	UPROPERTY()
	UStaticMesh* CylinderMesh;

	UPROPERTY()
	UStaticMesh* SphereMesh;

	EGatherResource GatherResource = EGatherResource::None;
	FLinearColor OriginalColor = FLinearColor::White;
	bool bDepleted = false;
	float RespawnAtTime = 0.0f;

	void TintMesh(UStaticMeshComponent* Mesh, const FLinearColor& Color) const;
};
