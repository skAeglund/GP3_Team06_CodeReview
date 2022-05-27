// Fill out your copyright notice in the Description page of Project Settings.


#include "TestDoor.h"
#include "GP3_2022_Team06/Generator.h"

// Sets default values
ATestDoor::ATestDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxComp"));
	Box->SetVisibility(true);
}

// Called when the game starts or when spawned
void ATestDoor::BeginPlay()
{
	TimeLeft = TimeUntilClose;
	for (int i : RequiredIDs)
	{
		Checks.Add(bool());
	}
	
	if (References->IsValidLowLevel())
	References->EnterGenArea.BindUObject(this, &ATestDoor::OpenDoor);
	Super::BeginPlay();
}

// Called every frame
void ATestDoor::Tick(float DeltaTime) // not really used right now
{
	bPassed = true;
	for (bool bCurrent : Checks)
	{
		if (bCurrent != true)
		{
			bPassed = false;
			break;
		}
	}
	if (bPassed)
	{
		Box->SetVisibility(false);
		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	if (TimeLeft <= 0)
	{
		Box->SetVisibility(true); // placeholder
		Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Closing = false;
		TimeLeft = TimeUntilClose;
		for (int i = 0; i < RequiredIDs.Num() ; i++)
		{
			Checks[i] = false;
		}
	}
	if (Closing)
	{
		TimeLeft = TimeLeft - DeltaTime;
	}
	Super::Tick(DeltaTime);

}
void ATestDoor::OpenDoor(int ID) //TODO: more generator tweaks required 
{
	for (int i = 0; i < RequiredIDs.Num() ; i++)
	{
		if (ID == RequiredIDs[i])
		{
			Checks[i] = true;
		}
	}
}
void ATestDoor::CloseDoor(int ID)
{
	Closing = true;
}

