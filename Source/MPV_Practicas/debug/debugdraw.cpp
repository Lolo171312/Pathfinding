#include "debugdraw.h"

#include "Subsystems/EditorActorSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/ArrowComponent.h"
#include "Math/Color.h"
#include "Kismet/KismetMathLibrary.h"


UActorComponent* GetComponentByName(const AActor* Actor, FName ComponentName)
{
    if (Actor)
    {
        // Iterate through all components attached to the actor
        TArray<UActorComponent*> Components;
        Actor->GetComponents(Components);

        for (UActorComponent* Component : Components)
        {
            if (Component && Component->GetFName() == ComponentName)
            {
                // Component with the specified name found
                return Component;
            }
        }

        // Component with the specified name not found
        UE_LOG(LogTemp, Warning, TEXT("Component with name %s not found in actor: %s"), *ComponentName.ToString(), *Actor->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid actor provided"));
    }

    return nullptr;
}



AActor* GetActorByLabelInEditor(const AActor* owner, const FString& label)
{
    // Get a reference to the editor actor subsystem
    UEditorActorSubsystem* EditorActorSubsystem = GEditor->GetEditorSubsystem<UEditorActorSubsystem>();

    if (EditorActorSubsystem)
    {
        // Use the editor actor subsystem
        // For example, you can spawn or manipulate actors
        auto all_actors = EditorActorSubsystem->GetAllLevelActors();

        for (AActor* Actor : all_actors)
        {
            // Check if the actor has a label
            FString ActorLabel = Actor->GetActorLabel();
            if (ActorLabel == label)
                return Actor;
        }
    }
    return NULL;
}

AActor* GetActorByLabel(const AActor* owner, const FString& label)
{
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(owner, AActor::StaticClass(), FoundActors);

    // Iterate over the found actors
    for (AActor* Actor : FoundActors)
    {
        // Do something with the actor
        // Check if the actor has a label
        FString ActorLabel = Actor->GetActorLabel();
        if (ActorLabel == label)
            return Actor;
    }
    return NULL;
}

void SetCircle(const AActor* owner, const FString& circle_name, const FVector& position, float radius)
{
    // Get Actor
    AActor* circle_actor = GetActorByLabel(owner, circle_name);
    if (!circle_actor)
        return;

    // Set position
    circle_actor->SetActorLocation(position);


    // Calculate scale from radius
    float newscale = radius / 100.0f;
    circle_actor->SetActorScale3D(FVector(newscale, newscale, newscale));
}

void SetArrow(const AActor* owner, const FString& arrow_name, const FVector& direction, float length)
{
    // Get Component
    UActorComponent* ActorComponent = GetComponentByName(owner, FName(arrow_name));
    UArrowComponent* ArrowComponent = Cast<UArrowComponent>(ActorComponent);

    if (ArrowComponent)
    {

        // Calculate angle from direction

        // Set rotation
        FVector arrow_dir = direction;
        arrow_dir.Normalize();
        float angle = atan2(arrow_dir.Z, arrow_dir.X);
        float deg = FMath::RadiansToDegrees(angle);

        FRotator rot(deg, 0.0f, 0.0f);
        ArrowComponent->SetRelativeRotation(rot);

        // Set Length
        ArrowComponent->ArrowLength = length;

        ArrowComponent->SetArrowColor(ArrowComponent->ArrowColor); // Force Update
    }
}