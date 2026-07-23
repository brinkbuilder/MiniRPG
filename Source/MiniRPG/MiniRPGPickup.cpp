#include "MiniRPGPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/Engine.h"

AMiniRPGPickup::AMiniRPGPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	// OverlapAllDynamic overlaps every object type regardless of the other
	// actor's own collision profile, so this reliably fires no matter what
	// bumps into it.
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(70.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RootComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetRelativeScale3D(FVector(0.5f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMeshAsset.Succeeded())
	{
		MeshComponent->SetStaticMesh(CubeMeshAsset.Object);
	}

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AMiniRPGPickup::OnOverlapBegin);
}

void AMiniRPGPickup::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AddActorLocalRotation(FRotator(0.0f, 90.0f * DeltaSeconds, 0.0f));
}

void AMiniRPGPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	UInventoryComponent* Inventory = OtherActor->FindComponentByClass<UInventoryComponent>();
	if (!Inventory)
	{
		return;
	}

	FInventoryItem Item;
	Item.Name = ItemName;
	Item.Type = PickupType;
	Item.Value = ItemValue;
	Inventory->AddItem(Item);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Picked up: %s"), *ItemName));
	}

	Destroy();
}
