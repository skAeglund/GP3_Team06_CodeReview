// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GP3_2022_Team06/Generator.h"
#include "TestDoor.generated.h"


UCLASS()
class GP3_2022_TEAM06_API ATestDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestDoor();

	virtual void Tick(float DeltaTime) override;
	class ATestTrigger* TriggerReference;
	UPROPERTY(EditAnywhere)
	AGenerator* References; // something is up with this one
	
	UPROPERTY(EditAnywhere)
	TArray<int32> RequiredIDs; // The IDs this actor needs to receive before proceeding
	TArray<bool> Checks; // keeps track of which IDs have been received

	UPROPERTY(EditAnywhere)
	float TimeUntilClose;
	
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Box;
	
	void OpenDoor(int);
	void CloseDoor(int);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool bPassed;
	bool Closing;
	float TimeLeft;
};
