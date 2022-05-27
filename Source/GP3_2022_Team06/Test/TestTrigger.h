// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestTrigger.generated.h"
DECLARE_DELEGATE_OneParam(APleaseWorkDelegate, int id);
UCLASS()
class GP3_2022_TEAM06_API ATestTrigger : public AActor // NO LONGER BEING USED
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestTrigger();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComp;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor);
	virtual void NotifyActorEndOverlap(AActor* OtherActor);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//APleaseWorkDelegate OnBoxEntered; 
	//APleaseWorkDelegate OnBoxExited;
	
	UPROPERTY(EditAnywhere)
	int32 SentID; // The ID that this trigger sends
	
	UPROPERTY(EditAnywhere)
	AActor *KeyActor;

};

