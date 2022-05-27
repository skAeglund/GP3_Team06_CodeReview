// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Pawn.h"
#include "Pickup.h"
#include "Camera/CameraComponent.h"
#include "PickupComponent.generated.h"

class USceneComponent;
class UBoxComponent;
class APickup;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3_2022_TEAM06_API UPickupComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPickupComponent();

	UFUNCTION()
	void ToggleItemPickup();

	UPROPERTY(VisibleAnywhere)
	bool bPickupInRange;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bHoldOnToObject;

	//UPROPERTY(EditAnywhere, Category = Holding)
	//UBoxComponent* BoxCollision;
	UPROPERTY(EditAnywhere, Category = Holding, meta = (ToolTip = "Should be found in instance, else generates an error"))
	USceneComponent* HoldComponent;

	UPROPERTY(EditAnywhere, Category = Holding, meta = (ToolTip = "Default holding distance from player, X"))
	float HoldDistance = 140.0f;

	UPROPERTY(EditAnywhere, Category = Holding, meta = (ToolTip = "Default holding offset from player, neg Y = left of player, pos Y = right of player"))
	float HoldOffset = 30.0f;

	UPROPERTY(EditAnywhere, Category = Holding, meta = (ToolTip = "Default holding height from player's middle position, Z"))
	float HoldHeight = 35.0f;

	UPROPERTY(EditAnywhere, Category = Holding, meta = (ToolTip = "Default holding distance for inspection, X, should be shorter than HoldDistance"))
	float InspectDistance = 110.0f;

	UPROPERTY(EditAnywhere, Category = Holding, meta = (ToolTip = "Speed hold item travels when close to obstacles"))
	float MoveHoldObjectSpeed = 8.0f;

	UPROPERTY(EditAnywhere, Category = Holding, meta = (ToolTip = "Default closest distance item can have from player, X"))
	float MaxDistanceToSelf = 40.0f;

	UPROPERTY(VisibleAnywhere, Category = Holding)
	float CurrentDistanceX;

	UPROPERTY(VisibleAnywhere, Category = Holding)
	float CurrentOffsetY;

	UPROPERTY(EditAnywhere, Category = Holding, meta = (ToolTip = "Offset from walls, so its doesnt goes trough them, X"))
	float DistanceOffset = 75.0f;

	/*UPROPERTY(VisibleAnywhere, Category = Lerp)
	float TargetDistance = 0;

	UPROPERTY(EditAnywhere, Category = Lerp)
	float LerpSpeed = 1.5f;
	float TimePassed = 0;*/

	UPROPERTY(EditAnywhere, Category = Holding)
	int ThrowAwayForce = 10000;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void OnAction();

	void OnInspect();
	void OnInspectReleased();

	void ToggleMovement();

	FVector GetLineTracePositionStart(bool IsCameraStart);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere)
	APickup* CurrentItem;


	UPROPERTY(EditAnywhere, meta = (ToolTip = "Should be found in instance, else generates an error"))
	UInputComponent* InputComp;

	bool bCanMove;
	bool bHoldingItem;
	bool bInspecting;

	float PitchMax;
	float PitchMin;

	FVector HoldingComp;
	FRotator LastRotation;

	FVector Start;
	FVector ForwardVector;
	FVector End;


	FComponentQueryParams DefaultComponentQueryParams;
	FCollisionResponseParams DefaultResponseParams;

	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere, Category = Inspecting)
	float DefaultCameraFOV = 90.0f;
	UPROPERTY(EditAnywhere, Category = Inspecting)
	float ZoomInCameraFOV = 45.0f;

	UPROPERTY(EditAnywhere, Category = Holding)
	float SearchOffset = 21.0f;

	UPROPERTY(EditAnywhere, Category = Holding)
	float PickupReach = 300.0f;
};
