// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UPickupComponent::UPickupComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	HoldComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HoldLocation"));
	
	CurrentDistanceX = HoldDistance;
	CurrentOffsetY = HoldOffset;
	CurrentItem = nullptr;
	bCanMove = true;
	bInspecting = false;

	bPickupInRange = false;
	bHoldOnToObject = false;
}

// Called when the game starts
void UPickupComponent::BeginPlay()
{
	Super::BeginPlay();
	HoldComponent->SetRelativeLocation(FVector(HoldDistance, HoldOffset, HoldHeight));
	PitchMax = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax;
	PitchMin = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin;

	CameraComponent = GetOwner()->FindComponentByClass<UCameraComponent>();

	if (!CameraComponent)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Red, FString::Printf(TEXT(
			"Error: Something is wrong, (camera not found) can you hear me, Major Tom? x3")));
	}

	InputComp = GetOwner()->InputComponent;

	if (InputComp)
	{
		InputComp->BindAction(TEXT("Interact"), IE_Pressed, this, &UPickupComponent::OnAction);
		InputComp->BindAction(TEXT("Inspect"), IE_Pressed, this, &UPickupComponent::OnInspect);
		InputComp->BindAction(TEXT("Inspect"), IE_Released, this, &UPickupComponent::OnInspectReleased);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Red, FString::Printf(TEXT(
			"Error: Something is wrong, (input from owner not found) can you hear me, Major Tom? x3")));
	}

}

// Called every frame
void UPickupComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Getting values for the line tracings
	if (!bHoldingItem)
	{
		Start = GetLineTracePositionStart(true);
		ForwardVector = CameraComponent->GetForwardVector();
	}
	else
	{
		Start = GetLineTracePositionStart(false);
		ForwardVector = HoldComponent->GetForwardVector() * -1;
	}
	End = ((ForwardVector * PickupReach) + Start);

	FHitResult Hit;
	//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 1, 0, 1);
	if (!bHoldingItem)
	{
		CurrentItem = nullptr;
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, DefaultComponentQueryParams,
			DefaultResponseParams))
		{
			// Is player aiming towards a pickup?
			if (Hit.GetActor()->GetClass()->IsChildOf(APickup::StaticClass()))
			{
				// Reset hold distance to avoid bugs
				//CurrentDistanceX = HoldDistance;
				//CurrentOffsetY = HoldOffset;
				CurrentItem = Cast<APickup>(Hit.GetActor());
				
			}
		}
	}
	else // if (bHoldingItem)
	{
		if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, DefaultComponentQueryParams,
			DefaultResponseParams))
		{
				/*GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Purple,
							FString::Printf(TEXT("Distance is %f"), Hit.Distance));*/
			// To close to wall
			if (Hit.Distance < DistanceOffset)
			{
				if (CurrentDistanceX > MaxDistanceToSelf)
				{
					CurrentDistanceX -= (Hit.Distance * MoveHoldObjectSpeed * DeltaTime);// +DistanceOffset;
				}
				if (CurrentOffsetY > (HoldOffset / 2))
				{
					//CurrentOffsetY -= Hit.Distance * DeltaTime;
				}
			}
			// If player moves away from wall and hit contact still active
			else if (MaxDistanceToSelf >= CurrentDistanceX && CurrentDistanceX < HoldDistance)
			{
				CurrentDistanceX += (Hit.Distance * MoveHoldObjectSpeed * DeltaTime);// +DistanceOffset;
			}
			//HoldLocation->SetRelativeLocation(FVector(CurrentDistanceX, CurrentOffsetY, HoldHeight));

		}
		// If player is away from wall and hit contact is not active
		else if (CurrentDistanceX < HoldDistance)
		{
			CurrentDistanceX += PickupReach * MoveHoldObjectSpeed * DeltaTime;
			//HoldLocation->SetRelativeLocation(FVector(CurrentDistanceX, CurrentOffsetY, HoldHeight));
			
		}
		else if (CurrentOffsetY < HoldOffset)
		{
			//CurrentOffsetY += Hit.Distance * DeltaTime;
		}
		// Clamping time, just to be safe
		CurrentDistanceX = FMath::Clamp(CurrentDistanceX, MaxDistanceToSelf, HoldDistance);
		CurrentOffsetY = FMath::Clamp(CurrentOffsetY, (HoldOffset / 2), HoldOffset);
		// Move component that holds the pickup
		HoldComponent->SetRelativeLocation(FVector(CurrentDistanceX, CurrentOffsetY, HoldHeight));

	}
	if (bInspecting)
	{
		// Rotate pickup during inspection mode
		if (bHoldingItem)
		{
			CameraComponent->SetFieldOfView(FMath::Lerp(CameraComponent->FieldOfView, DefaultCameraFOV, 0.1f));
			
			HoldComponent->SetRelativeLocation(FVector(InspectDistance, HoldOffset, HoldHeight));
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = 179.9000002f;
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin = -179.9000002f;
			CurrentItem->RotateActor();
		}
		else
		{
			CameraComponent->SetFieldOfView(FMath::Lerp(CameraComponent->FieldOfView, ZoomInCameraFOV, 0.1f));
		}
	}
	else
	{
		CameraComponent->SetFieldOfView(FMath::Lerp(CameraComponent->FieldOfView, DefaultCameraFOV, 0.1f));

		if (bHoldingItem)
		{
			HoldComponent->SetRelativeLocation(FVector(CurrentDistanceX, CurrentOffsetY, HoldHeight));
		}
	}

	if (CurrentItem)
	{
		
		if (CurrentItem->bDropped)
		{
			//	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 4.f, FColor::Blue,
			//		FString::Printf(TEXT("Done, Distance is %f"), CurrentDistance));
			bHoldingItem = false;
			CurrentItem->bDropped = false;
			CurrentItem = nullptr;
		}
	}
}

void UPickupComponent::OnAction()
{
	if (CurrentItem && !bInspecting && !bHoldOnToObject)
	{
		ToggleItemPickup();
	}
}

void UPickupComponent::OnInspect()
{
	if (bHoldingItem)
	{
		LastRotation = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetControlRotation();
		ToggleMovement();
	}
	else
	{
		bInspecting = true;
	}
}

void UPickupComponent::OnInspectReleased()
{
	if (bInspecting && bHoldingItem)
	{
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetControlRotation(LastRotation);
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMax = PitchMax;
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->ViewPitchMin = PitchMin;
		ToggleMovement();
	}
	else
	{
		bInspecting = false;
	}
}

void UPickupComponent::ToggleMovement()
{
	bCanMove = !bCanMove;
	bInspecting = !bInspecting;
	CameraComponent->bUsePawnControlRotation = ~CameraComponent->bUsePawnControlRotation;
	
	if (GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		GetWorld()->GetFirstPlayerController()->GetPawn()->bUseControllerRotationYaw
			= ~GetWorld()->GetFirstPlayerController()->GetPawn()->bUseControllerRotationYaw;
	}
}
// Todo make this T thing instead of bool parameter, temp thing before real asset gets in the game
FVector UPickupComponent::GetLineTracePositionStart(bool IsCameraStart)
{
	if (IsCameraStart)
	{
		return CameraComponent->GetComponentLocation() + (CameraComponent->GetForwardVector() * SearchOffset);
	}
	else
	{
		return HoldComponent->GetComponentLocation() + (HoldComponent->GetForwardVector() * -SearchOffset);

	}
}

void UPickupComponent::ToggleItemPickup()
{
	if (CurrentItem)
	{
		bHoldingItem = !bHoldingItem;
		CurrentItem->Interact(ThrowAwayForce);
		
		if (!bHoldingItem)
		{
			CurrentItem = nullptr;
		}
	}
}





