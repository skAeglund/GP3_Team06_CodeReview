// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

class UPlayerPawnMovementComponent;
UCLASS()
class GP3_2022_TEAM06_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
	UPROPERTY(EditAnywhere)
	UCapsuleComponent* PlayerCollider;
	UPROPERTY(EditAnywhere)
	USceneComponent* PlayerMesh;
	
	UPROPERTY(EditAnywhere)
	UCameraComponent* PlayerCamera;

	UPROPERTY(VisibleAnywhere)
	UPlayerPawnMovementComponent* MoveComp;
	
	//Input variables
	FVector PlayerInput;
	FVector CameraInput;
	float ZoomFactor;
	bool bZoomingIn;
	
	bool IsGrounded = false;

	void HandleMoveForward(float Value);
	void HandleMoveRight(float Value);
	void Handle();
	void PitchCamera(float Value);
	void YawCamera(float Value);

	
};
