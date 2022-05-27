// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "PlayerPawnMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	
	MoveComp = CreateDefaultSubobject<UPlayerPawnMovementComponent>(TEXT("MovementComponent"));
	PlayerCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Player Collider"));
	PlayerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Player Mesh"));
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Player Camera"));
	
	RootComponent = PlayerCollider;
	PlayerCollider->SetCollisionResponseToAllChannels(ECR_Block);
	
	PlayerCamera->SetupAttachment(PlayerCollider,USpringArmComponent::SocketName);
}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PlayerCamera->AddLocalRotation(FRotator(-CameraInput.Y, 0 , 0));
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerPawn::HandleMoveForward);
	InputComponent->BindAxis(TEXT("MoveRight"), this, &APlayerPawn::HandleMoveRight);
	InputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &APlayerPawn::Handle);
	InputComponent->BindAxis("LookUp", this, &APlayerPawn::PitchCamera);
	InputComponent->BindAxis("Turn", this, &APlayerPawn::YawCamera);
}

void APlayerPawn::Handle()
{
	if (MoveComp->OnGround())
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE,1,FColor::Green,FString::Printf(TEXT("JumpRequest"),*GetActorLocation().ToString()));
		MoveComp->RequestJump = true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE,1,FColor::Yellow,FString::Printf(TEXT("Need to be grounded to jump"),*GetActorLocation().ToString()));
	}
	//Jumping = true;
}

void APlayerPawn::HandleMoveForward(float Value)
{
	GEngine->AddOnScreenDebugMessage
	(
		INDEX_NONE,
		0.f,
		FColor::Green,
		FString::Printf(TEXT("MoveForward: %f"), Value)
	);
	
	MoveComp->PlayerInput.X = PlayerInput.X = FMath::Clamp(Value, -1.0f, 1.0f);
}

void APlayerPawn::HandleMoveRight(float Value)
{
	GEngine->AddOnScreenDebugMessage
	(
		INDEX_NONE,
		0.f,
		FColor::Green,
		FString::Printf(TEXT("MoveRight: %f"), Value)
	);
	
	MoveComp->PlayerInput.Y = PlayerInput.Y = FMath::Clamp(Value, -1.0f, 1.0f);
}

void APlayerPawn::PitchCamera(float Value)
{
	GEngine->AddOnScreenDebugMessage(
	INDEX_NONE,
	0.f,
	FColor::Green,
	FString::Printf(TEXT("LookUp: %f"), Value));
	
	MoveComp->CameraInput.Y = CameraInput.Y = Value;
}

void APlayerPawn::YawCamera(float Value)
{
	GEngine->AddOnScreenDebugMessage(
	INDEX_NONE,
	0.f,
	FColor::Green,
	FString::Printf(TEXT("Turn: %f"), Value));
	
	MoveComp->CameraInput.X = CameraInput.X = Value;
}


