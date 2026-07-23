#include "MiniRPGPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "RPGStatsComponent.h"
#include "InventoryComponent.h"
#include "MiniRPGEnemyCharacter.h"
#include "MiniRPGProp.h"
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
	SpringArm->TargetArmLength = 800.0f;
	SpringArm->SetRelativeRotation(FRotator(-55.0f, 0.0f, 0.0f));
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
	PlayerInputComponent->BindAction("Click", IE_Pressed, this, &AMiniRPGPlayerCharacter::OnClickPressed);
}

void AMiniRPGPlayerCharacter::MoveForward(float Value)
{
	if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		CancelAutoAction();
		AddMovementInput(FVector::ForwardVector, Value);
	}
}

void AMiniRPGPlayerCharacter::MoveRight(float Value)
{
	if (FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		CancelAutoAction();
		AddMovementInput(FVector::RightVector, Value);
	}
}

void AMiniRPGPlayerCharacter::CancelAutoAction()
{
	bHasMoveTarget = false;
	AttackTarget = nullptr;
	GatherTarget = nullptr;
}

void AMiniRPGPlayerCharacter::OnClickPressed()
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC)
	{
		return;
	}

	FHitResult Hit;
	if (!PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit) || !Hit.bBlockingHit)
	{
		return;
	}

	CancelAutoAction();

	if (AMiniRPGEnemyCharacter* Enemy = Cast<AMiniRPGEnemyCharacter>(Hit.GetActor()))
	{
		if (Enemy->StatsComponent && !Enemy->StatsComponent->IsDead())
		{
			AttackTarget = Enemy;
			return;
		}
	}

	if (AMiniRPGProp* Prop = Cast<AMiniRPGProp>(Hit.GetActor()))
	{
		if (Prop->IsGatherable())
		{
			GatherTarget = Prop;
			return;
		}
	}

	MoveTargetLocation = Hit.Location;
	bHasMoveTarget = true;
}

void AMiniRPGPlayerCharacter::MoveToward(const FVector& TargetLocation)
{
	FVector Direction = TargetLocation - GetActorLocation();
	Direction.Z = 0.0f;
	if (!Direction.IsNearlyZero())
	{
		AddMovementInput(Direction.GetSafeNormal(), 1.0f);
	}
}

void AMiniRPGPlayerCharacter::UpdateAutoActions()
{
	if (StatsComponent && StatsComponent->IsDead())
	{
		CancelAutoAction();
		return;
	}

	if (AttackTarget.IsValid())
	{
		if (!AttackTarget->StatsComponent || AttackTarget->StatsComponent->IsDead())
		{
			AttackTarget = nullptr;
			return;
		}

		const float Dist = FVector::Dist(GetActorLocation(), AttackTarget->GetActorLocation());
		if (Dist > AttackRange)
		{
			MoveToward(AttackTarget->GetActorLocation());
		}
		else
		{
			DealDamageToEnemy(AttackTarget.Get());
		}
		return;
	}

	if (GatherTarget.IsValid())
	{
		if (!GatherTarget->IsGatherable())
		{
			// A rock went dark mid-approach -- give up on it rather than
			// stand there waiting for a respawn.
			GatherTarget = nullptr;
			return;
		}

		const float Dist = FVector::Dist(GetActorLocation(), GatherTarget->GetActorLocation());
		if (Dist > GatherRange)
		{
			MoveToward(GatherTarget->GetActorLocation());
		}
		else
		{
			TryGather(GatherTarget.Get());
		}
		return;
	}

	if (bHasMoveTarget)
	{
		const float Dist = FVector::Dist2D(GetActorLocation(), MoveTargetLocation);
		if (Dist > 30.0f)
		{
			MoveToward(MoveTargetLocation);
		}
		else
		{
			bHasMoveTarget = false;
		}
	}
}

void AMiniRPGPlayerCharacter::OnAttackPressed()
{
	if (!StatsComponent || StatsComponent->IsDead())
	{
		return;
	}

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), AttackRange, ObjectTypes,
		AMiniRPGEnemyCharacter::StaticClass(), IgnoreActors, OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (AMiniRPGEnemyCharacter* Enemy = Cast<AMiniRPGEnemyCharacter>(Actor))
		{
			DealDamageToEnemy(Enemy);
			break;
		}
	}
}

void AMiniRPGPlayerCharacter::DealDamageToEnemy(AMiniRPGEnemyCharacter* Enemy)
{
	if (!Enemy || !Enemy->StatsComponent || Enemy->StatsComponent->IsDead() || !StatsComponent)
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastAttackTime < AttackCooldown)
	{
		return;
	}
	LastAttackTime = Now;

	Enemy->StatsComponent->TakeDamage(StatsComponent->AttackPower);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Orange, TEXT("Hit!"));
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
		AttackTarget = nullptr;
		Enemy->Destroy();
	}
}

void AMiniRPGPlayerCharacter::TryGather(AMiniRPGProp* Prop)
{
	if (!Prop || !Prop->IsGatherable())
	{
		GatherTarget = nullptr;
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();
	if (Now - LastGatherTime < GatherCooldown)
	{
		return;
	}
	LastGatherTime = Now;

	if (Prop->GetGatherResource() == EGatherResource::Wood)
	{
		WoodcuttingSkill.GainXP(8);
		if (InventoryComponent)
		{
			FInventoryItem Item;
			Item.Name = TEXT("Logs");
			Item.Type = EItemType::Wood;
			Item.Value = 1;
			InventoryComponent->AddItem(Item);
		}
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, TEXT("You chop some logs."));
		}
	}
	else if (Prop->GetGatherResource() == EGatherResource::Ore)
	{
		MiningSkill.GainXP(12);
		if (InventoryComponent)
		{
			FInventoryItem Item;
			Item.Name = TEXT("Ore");
			Item.Type = EItemType::Ore;
			Item.Value = 1;
			InventoryComponent->AddItem(Item);
		}
		Prop->Deplete(8.0f);
		GatherTarget = nullptr;
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Green, TEXT("You mine some ore. The rock crumbles."));
		}
	}
}

void AMiniRPGPlayerCharacter::OnUseItemPressed()
{
	if (!InventoryComponent)
	{
		return;
	}

	const int32 PotionIndex = InventoryComponent->FindFirstOfType(EItemType::Potion);
	if (PotionIndex != INDEX_NONE)
	{
		InventoryComponent->UseItem(PotionIndex);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Used a potion."));
		}
	}
}

void AMiniRPGPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateAutoActions();

	if (!GEngine || !StatsComponent)
	{
		return;
	}

	if (StatsComponent->IsDead())
	{
		GEngine->AddOnScreenDebugMessage(0, 0.0f, FColor::Red, TEXT("You died. Press Play again to restart."));
		return;
	}

	GEngine->AddOnScreenDebugMessage(0, 0.0f, FColor::Yellow, FString::Printf(
		TEXT("Combat Lv %d | HP %d/%d | ATK %d | DEF %d | XP %d/%d"),
		StatsComponent->Level, StatsComponent->CurrentHealth, StatsComponent->MaxHealth,
		StatsComponent->AttackPower, StatsComponent->Defense,
		StatsComponent->Experience, StatsComponent->ExperienceToNextLevel));

	GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Green, FString::Printf(
		TEXT("Woodcutting Lv %d (XP %d/%d) | Mining Lv %d (XP %d/%d)"),
		WoodcuttingSkill.Level, WoodcuttingSkill.XP, WoodcuttingSkill.XPToNextLevel,
		MiningSkill.Level, MiningSkill.XP, MiningSkill.XPToNextLevel));

	int32 Logs = 0, Ore = 0, Potions = 0, Upgrades = 0;
	for (const FInventoryItem& Item : InventoryComponent->Items)
	{
		switch (Item.Type)
		{
		case EItemType::Wood: Logs++; break;
		case EItemType::Ore: Ore++; break;
		case EItemType::Potion: Potions++; break;
		case EItemType::WeaponUpgrade: Upgrades++; break;
		}
	}
	GEngine->AddOnScreenDebugMessage(2, 0.0f, FColor::White, FString::Printf(
		TEXT("Inventory -- Logs: %d | Ore: %d | Potions: %d | Upgrades: %d"), Logs, Ore, Potions, Upgrades));

	GEngine->AddOnScreenDebugMessage(3, 0.0f, FColor::White,
		TEXT("Left-click: ground to walk, an enemy to fight, a tree/rock to gather. WASD also moves. Space=attack, E=use potion."));
}
