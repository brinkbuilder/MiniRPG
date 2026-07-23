#include "MiniRPGProp.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

AMiniRPGProp::AMiniRPGProp()
{
	// Always tickable (cheap early-out below) so Deplete() can be called on
	// any prop without needing bCanEverTick flipped after construction --
	// SetActorTickEnabled() alone can't register a tick that started disabled.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;
	BaseMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	TopMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopMesh"));
	TopMesh->SetupAttachment(RootComponent);
	TopMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TopMesh->SetVisibility(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConeAsset(TEXT("/Engine/BasicShapes/Cone.Cone"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));

	if (CubeAsset.Succeeded()) { CubeMesh = CubeAsset.Object; }
	if (ConeAsset.Succeeded()) { ConeMesh = ConeAsset.Object; }
	if (CylinderAsset.Succeeded()) { CylinderMesh = CylinderAsset.Object; }
	if (SphereAsset.Succeeded()) { SphereMesh = SphereAsset.Object; }

	BaseMesh->SetStaticMesh(CylinderMesh);
}

void AMiniRPGProp::TintMesh(UStaticMeshComponent* Mesh, const FLinearColor& Color) const
{
	if (!Mesh)
	{
		return;
	}
	if (UMaterialInstanceDynamic* Dyn = Mesh->CreateAndSetMaterialInstanceDynamic(0))
	{
		Dyn->SetVectorParameterValue(TEXT("Color"), Color);
	}
}

void AMiniRPGProp::ConfigureAsTree(float TrunkHeight, const FLinearColor& TrunkColor, const FLinearColor& LeafColor)
{
	BaseMesh->SetStaticMesh(CylinderMesh);
	BaseMesh->SetRelativeScale3D(FVector(0.35f, 0.35f, TrunkHeight));
	TintMesh(BaseMesh, TrunkColor);

	TopMesh->SetVisibility(true);
	TopMesh->SetStaticMesh(ConeMesh);
	TopMesh->SetRelativeLocation(FVector(0.0f, 0.0f, TrunkHeight * 80.0f));
	TopMesh->SetRelativeScale3D(FVector(2.4f, 2.4f, 2.6f));
	TintMesh(TopMesh, LeafColor);

	GatherResource = EGatherResource::Wood;
}

void AMiniRPGProp::ConfigureAsRock(float Scale, const FLinearColor& Color)
{
	TopMesh->SetVisibility(false);
	BaseMesh->SetStaticMesh(SphereMesh);
	BaseMesh->SetRelativeScale3D(FVector(Scale));
	OriginalColor = Color;
	TintMesh(BaseMesh, Color);

	GatherResource = EGatherResource::Ore;
}

void AMiniRPGProp::Deplete(float RespawnSeconds)
{
	if (GatherResource != EGatherResource::Ore)
	{
		return;
	}

	bDepleted = true;
	RespawnAtTime = GetWorld()->GetTimeSeconds() + RespawnSeconds;
	TintMesh(BaseMesh, FLinearColor(0.12f, 0.12f, 0.12f));
}

void AMiniRPGProp::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bDepleted)
	{
		return;
	}

	if (GetWorld()->GetTimeSeconds() >= RespawnAtTime)
	{
		bDepleted = false;
		TintMesh(BaseMesh, OriginalColor);
	}
}
