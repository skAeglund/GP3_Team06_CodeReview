// Fill out your copyright notice in the Description page of Project Settings.

#include "TestTrigger.h"
#include "Components/BoxComponent.h"

// Sets default values
ATestTrigger::ATestTrigger()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetBoxExtent(FVector(128, 128, 64));
	BoxComp->SetVisibility(true);

}

// Called when the game starts or when spawned
void ATestTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATestTrigger::NotifyActorEndOverlap(AActor* OtherActor)
{
	if (OtherActor == KeyActor)
	{
		//OnBoxExited.ExecuteIfBound(SentID);
	}
}

void ATestTrigger::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (OtherActor == KeyActor)
	{
		//OnBoxEntered.ExecuteIfBound(SentID);
	}
}
