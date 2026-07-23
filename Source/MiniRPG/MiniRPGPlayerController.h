#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MiniRPGPlayerController.generated.h"

// Shows and unlocks the mouse cursor and enables click-through gameplay
// input alongside it, so the player can point-and-click to move, attack,
// and gather while WASD/Space/E still work.
UCLASS()
class AMiniRPGPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
