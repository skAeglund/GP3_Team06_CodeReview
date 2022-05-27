// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Pickup.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UBoxComponent;
class UPickupComponent;

UCLASS()
class GP3_2022_TEAM06_API APickup : public AActor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MyMesh;
	UPROPERTY(VisibleAnywhere, Category = Holding)
	USceneComponent* HoldingComp;

	//UPROPERTY(VisibleAnywhere, Category = Holding)
	//UBoxComponent* BoxCollision;
	
	UPROPERTY(EditAnywhere, Category = Holding)
	int CollisionDropForce = 10;
	UPROPERTY(VisibleAnywhere, Category = Holding)
	bool bHolding;

	UPROPERTY(VisibleAnywhere)
	bool bGravity;
	UPROPERTY(VisibleAnywhere)
	bool bDropped;
	UPROPERTY(EditAnywhere)
	UPickupComponent* PickupComp;

	FRotator ControlRotation;
	APawn* PlayerPawn;
	UCameraComponent* PlayerCamera;
	FVector ForwardVector;

	// Sets default values for this actor's properties
	APickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void RotateActor();

	UFUNCTION()
	bool Interact(int ThrowAwayForce);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
