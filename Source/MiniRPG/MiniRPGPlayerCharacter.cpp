#include "MiniRPGPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "RPGStatsComponent.h"
#include "InventoryComponent.h"
#include "MiniRPGEnemyCharacter.h"
#include "MiniRPGGameMode.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine/EngineTypes.h"
#include "Engine/Engine.h"

AMiniRPGPlayerCharacter::AMiniRPGPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(34.0f, 88.0f);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = 500.0f;

	// Engine-bundled tutorial character mesh + anim blueprint (idle/walk
	// blend space driven by speed) -- ships with any UE install, no
	// content needs to be authored or imported.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(
		TEXT("/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialTPP.TutorialTPP"));
	if (MeshAsset.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPClass(
		TEXT("/Engine/Tutorial/SubEditors/TutorialAssets/Character/TutorialTPP_AnimBlueprint"));
	if (AnimBPClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);
	}
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	if (UMaterialInstanceDynamic* Dyn = GetMesh()->CreateAndSetMaterialInstanceDynamic(0))
	{
		Dyn->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.15f, 0.35f, 0.85f));
	}

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 700.0f;
	SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	SpringArm->bDoCollisionTest = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 6.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	StatsComponent = CreateDefaultSubobject<URPGStatsComponent>(TEXT("StatsComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AMiniRPGPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMiniRPGPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMiniRPGPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AMiniRPGPlayerCharacter::OnAttackPressed);
	PlayerInputComponent->BindAction("UseItem", IE_Pressed, this, &AMiniRPGPlayerCharacter::OnUseItemPressed);
}

void AMiniRPGPlayerCharacter::MoveForward(float Value)
{
	AddMovementInput(FVector::ForwardVector, Value);
}

void AMiniRPGPlayerCharacter::MoveRight(float Value)
{
	AddMovementInput(FVector::RightVector, Value);
}

void AMiniRPGPlayerCharacter::OnAttackPressed()
{
	if (!StatsComponent || StatsComponent->IsDead())
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastAttackTime < AttackCooldown)
	{
		return;
	}
	LastAttackTime = Now;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), AttackRange, ObjectTypes,
		AMiniRPGEnemyCharacter::StaticClass(), IgnoreActors, OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		AMiniRPGEnemyCharacter* Enemy = Cast<AMiniRPGEnemyCharacter>(Actor);
		if (!Enemy || !Enemy->StatsComponent || Enemy->StatsComponent->IsDead())
		{
			continue;
		}

		Enemy->StatsComponent->TakeDamage(StatsComponent->AttackPower);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Hit!"));
		}

		if (Enemy->StatsComponent->IsDead())
		{
			StatsComponent->GainExperience(Enemy->ExperienceReward);
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("Enemy defeated!"));
			}
			if (AMiniRPGGameMode* GM = GetWorld()->GetAuthGameMode<AMiniRPGGameMode>())
			{
				GM->NotifyEnemyDefeated();
			}
			Enemy->Destroy();
		}
		break;
	}
}

void AMiniRPGPlayerCharacter::OnUseItemPressed()
{
	if (InventoryComponent && InventoryComponent->Items.Num() > 0)
	{
		InventoryComponent->UseItem(0);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Used an item."));
		}
	}
}

void AMiniRPGPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GEngine && StatsComponent)
	{
		const FString StatusLine = StatsComponent->IsDead()
			? TEXT("You died. Press Play again to restart.")
			: FString::Printf(TEXT("Lv %d | HP %d/%d | ATK %d | DEF %d | XP %d/%d | Items: %d  (Space=Attack, E=Use Item)"),
				StatsComponent->Level, StatsComponent->CurrentHealth, StatsComponent->MaxHealth,
				StatsComponent->AttackPower, StatsComponent->Defense,
				StatsComponent->Experience, StatsComponent->ExperienceToNextLevel,
				InventoryComponent ? InventoryComponent->Items.Num() : 0);

		GEngine->AddOnScreenDebugMessage(0, 0.0f, FColor::Yellow, StatusLine);
	}
}
