#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MiniRPGProp.generated.h"

class UStaticMeshComponent;

// A simple decorative actor (tree or rock) assembled from engine basic
// shapes at runtime -- purely visual set dressing, no custom content.
UCLASS()
class AMiniRPGProp : public AActor
{
	GENERATED_BODY()

public:
	AMiniRPGProp();

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	UStaticMeshComponent* TopMesh;

	void ConfigureAsTree(float TrunkHeight, const FLinearColor& TrunkColor, const FLinearColor& LeafColor);
	void ConfigureAsRock(float Scale, const FLinearColor& Color);

private:
	UPROPERTY()
	UStaticMesh* CubeMesh;

	UPROPERTY()
	UStaticMesh* ConeMesh;

	UPROPERTY()
	UStaticMesh* CylinderMesh;

	UPROPERTY()
	UStaticMesh* SphereMesh;

	void TintMesh(UStaticMeshComponent* Mesh, const FLinearColor& Color) const;
};
