# MInteraction Plugin

A flexible and efficient interaction system for Unreal Engine that handles player interactions with in-game objects. Automatically detects interactable objects in range and manages the interaction focus and lifecycle.

## Overview

MInteraction provides a complete framework for implementing item pickups, door interactions, NPC conversations, and any other interaction-based gameplay mechanics. The system is network-ready and includes built-in UI support for displaying interaction prompts.

### Core Concept

The plugin operates on a simple but powerful principle:
- **Interactors** (typically the player) search for **Interactables** (in-game objects) within a configurable range
- The system automatically determines which interactable should receive focus based on proximity and direction
- When the interaction button is pressed, the focused interactable is triggered
- Events are fired throughout the lifecycle, allowing complete customization without modifying core code

## Features

- **Automatic Detection**: Finds all interactable objects in range using efficient spatial queries
- **Smart Focus Selection**: Automatically selects the nearest/most relevant interactable
- **Focus Events**: Notifies when interactable focus changes for UI and gameplay updates
- **UI System**: Built-in widget pooling system for displaying interaction prompts above interactables
- **Extensible Architecture**: Blueprint-implementable events for custom behavior
- **Function Library**: Blueprint-accessible utility functions for common tasks
- **State Management**: Enable/disable interactions dynamically
- **Blueprint Native Events**: Implement custom interaction logic in both C++ and Blueprints

## Installation

1. Add the MInteraction plugin to your project's `Plugins` folder
2. Add plugin reference to your `.Build.cs` file:
   ```csharp
   PublicDependencyModuleNames.AddRange(new string[] { "MInteraction" });
   ```
3. Compile and reload the project

### Dependencies

- **MUtility** - Core utility functions
- **MPointSelectorQuery** - Point selection and query filtering system
- **SDFutureExtensions** - Async operation support

## Quick Start

### Setting Up an Interactor (Player)

1. Add `UMIInteractorComponent` to your player pawn:

```cpp
// MyCharacter.h
#include "MIInteractorComponent.h"

UCLASS()
class AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyCharacter();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UMIInteractorComponent* InteractorComponent;
};

// MyCharacter.cpp
AMyCharacter::AMyCharacter()
{
    InteractorComponent = CreateDefaultSubobject<UMIInteractorComponent>(TEXT("Interactor"));
}
```

2. Setup input binding for interaction:

```cpp
void AMyCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    
    PlayerInputComponent->BindAction("Interact", IE_Pressed, InteractorComponent, &UMIInteractorComponent::Interact);
}
```

### Creating an Interactable Object

1. Add `UMIInteractableComponent` to your interactable actor:

```cpp
// MyPickupItem.h
#include "MIInteractableComponent.h"

UCLASS()
class AMyPickupItem : public AActor
{
    GENERATED_BODY()

public:
    AMyPickupItem();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    class UMIInteractableComponent* InteractableComponent;

    UFUNCTION()
    void OnItemInteracted(const FMIOnInteractedData& InteractedData);
};

// MyPickupItem.cpp
AMyPickupItem::AMyPickupItem()
{
    InteractableComponent = CreateDefaultSubobject<UMIInteractableComponent>(TEXT("Interactable"));
    Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    
    InteractableComponent->OnInteractedDelegate.AddDynamic(this, &AMyPickupItem::OnItemInteracted);
}

void AMyPickupItem::OnItemInteracted(const FMIOnInteractedData& InteractedData)
{
    // Handle the interaction
    UE_LOG(LogTemp, Warning, TEXT("Item picked up by %s"), *InteractedData.Instigator->GetName());
    Destroy();
}
```

## Architecture

### Main Components

#### UMIInteractorComponent
Attached to the player (or any actor that can interact). Responsible for:
- Searching for interactables in range
- Determining the current focus target
- Triggering interactions
- Firing focus events

**Key Methods:**
- `Interact()` - Triggers interaction with focused interactable
- `GetInteractableFocused()` - Returns currently focused interactable
- `GetInteractablesInRange()` - Returns all interactables within range

**Key Events:**
- `OnInteractableFocusBegin(Interactable)` - Called when focus changes to a new interactable
- `OnInteractableFocusLost(Interactable)` - Called when focus is lost

#### UMIInteractableComponent
Attached to interactable actors. Responsible for:
- Implementing interactable interface
- Managing interactable state (enabled/disabled, focused/unfocused)
- Firing interaction events
- Validating interaction permissions

**Key Methods:**
- `Interact_Implementation(Instigator)` - Called when player interacts
- `CanInteract_Implementation(Instigator)` - Validates if interaction is allowed
- `EnableInteraction_Implementation()` / `DisableInteraction_Implementation()` - Controls interaction availability

**Key Events:**
- `OnInteractedDelegate` - Fired after successful interaction

#### IMIInteractableInterface
Interface implemented by both component and custom classes for interaction functionality.

### UI System

#### UMInteractorWidgetComponent
Manages widget display for all interactables in range. Features:
- Automatic widget pooling for performance
- Screen space projection of world positions
- Handles show/hide based on focus state

#### UMInteractableInRangeWidget
Abstract widget class for displaying interaction prompts. Can be extended in Blueprints or C++:
- `SetInteractable()` - Assigns the interactable to this widget
- `SetFocused(bool)` - Called when focus state changes
- Blueprint events: `OnCreated`, `OnFocusChanged`, `OnRemoved`

## Advanced Usage

### Blueprint-Based Interaction Handler

Create a custom interactable in Blueprint by implementing `IMIInteractableInterface`:

```
In Blueprint:
1. Create Actor Blueprint
2. Add Component of type "MIInteractableComponent"
3. Bind to OnInteracted event
4. Implement custom logic in the delegate callback
```

### Custom Interaction Criteria

Override interactor focus selection by extending `UMIInteractorComponent`:

```cpp
UCLASS()
class MYGAME_API AMyCustomCharacter : public ACharacter
{
    GENERATED_BODY()

protected:
    UCLASS()
    class UMyCustomInteractor : public UMIInteractorComponent
    {
        GENERATED_BODY()

    protected:
        virtual void OnInteractableFocusBegin_Implementation(
            const TScriptInterface<IMIInteractableInterface>& Interactable) override;
        virtual void OnInteractableFocusLost_Implementation(
            const TScriptInterface<IMIInteractableInterface>& Interactable) override;
    };
};
```

### Using the Function Library

Perform quick interaction checks from anywhere:

```cpp
#include "MIInteractableFunctionLibrary.h"

// Check if an actor is interactable
if (UMIInteractableFunctionLibrary::IsActorInteractable(MyActor))
{
    // Perform interaction
    UMIInteractableFunctionLibrary::Interact(MyActor);
}

// Get interactable from actor
auto Interactable = UMIInteractableFunctionLibrary::GetInteractableFromActor(MyActor);
if (Interactable.GetObject())
{
    IIMIInteractableInterface::Execute_EnableInteractableFocused(Interactable.GetObject());
}
```

## Configuration

### Interactor Range
Configure the interaction range using a **MPointSelectorQuery** settings asset:

1. Create a new `MPointSelectorQuerySettingsAsset` in your Content Browser
2. Configure query radius and other filtering options
3. Assign to `QuerySettingsAsset` property on the Interactor component

### Widget Display
Customize widget appearance and behavior:
- Set `InteractableInRangeClass` to your custom widget class
- Adjust `InitialPoolSize` for performance tuning (number of pre-allocated widgets)

## Example: Complete Door Interaction

```cpp
// MyDoor.h
#include "GameFramework/Actor.h"
#include "MIInteractableComponent.h"
#include "MyDoor.generated.h"

UCLASS()
class AMyDoor : public AActor
{
    GENERATED_BODY()

public:
    AMyDoor();

    virtual void BeginPlay() override;

protected:
    UPROPERTY(VisibleAnywhere)
    UMIInteractableComponent* InteractableComponent;

    UPROPERTY(VisibleAnywhere)
    class UTimelineComponent* DoorTimeline;

    UPROPERTY(EditAnywhere)
    bool bIsLocked = false;

    UPROPERTY(EditAnywhere)
    float DoorOpenAngle = 90.0f;

    UFUNCTION()
    void OnDoorInteracted(const FMIOnInteractedData& Data);

    UFUNCTION()
    void OnDoorTimelineUpdate(float Value);

    bool bIsOpen = false;
};

// MyDoor.cpp
#include "MyDoor.h"
#include "Components/TimelineComponent.h"

AMyDoor::AMyDoor()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    InteractableComponent = CreateDefaultSubobject<UMIInteractableComponent>(TEXT("Interactable"));
    InteractableComponent->SetupAttachment(RootComponent);

    DoorTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("DoorTimeline"));
    DoorTimeline->SetupAttachment(RootComponent);
}

void AMyDoor::BeginPlay()
{
    Super::BeginPlay();
    InteractableComponent->OnInteractedDelegate.AddDynamic(this, &AMyDoor::OnDoorInteracted);
}

void AMyDoor::OnDoorInteracted(const FMIOnInteractedData& Data)
{
    if (bIsLocked)
    {
        UE_LOG(LogTemp, Warning, TEXT("Door is locked!"));
        return;
    }

    bIsOpen = !bIsOpen;
    // Play door animation
    DoorTimeline->Play();
}

void AMyDoor::OnDoorTimelineUpdate(float Value)
{
    float TargetAngle = bIsOpen ? DoorOpenAngle : 0.0f;
    RootComponent->SetRelativeRotation(FRotator(0, FMath::Lerp(0, TargetAngle, Value), 0));
}
```

## Performance Considerations

- **Widget Pooling**: Widgets are reused to minimize allocation overhead
- **State Management**: Disable interactions when not needed to skip focus checks


