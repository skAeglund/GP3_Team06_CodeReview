// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "PickupComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyMesh"));
	MyMesh->SetSimulatePhysics(true);
	RootComponent = MyMesh;

	//BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapCollision"));
	//BoxCollision->SetBoxExtent(FVector(30, 30, 50));
	////BoxCollision->SetupAttachment(MyMesh);
	////BoxCollision->SetRelativeLocation(FVector(0, 0, 0));

	//BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnOverlapBegin);
	//BoxCollision->OnComponentEndOverlap.AddDynamic(this, &APickup::OnOverlapEnd);

	bHolding = false;
	bGravity = true;
	bDropped = false;
}


// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0); //GetPlayerCharacter(

	TArray<USceneComponent*> Components;

	if (PlayerPawn)
	{
		PlayerPawn->GetComponents(Components);
		PlayerCamera = PlayerPawn->FindComponentByClass<UCameraComponent>();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 5.f, FColor::Red, FString::Printf(TEXT(
			"Error: Something is wrong, (character not found) can you hear me, Major Tom? x3")));
		return;
	}

	if (Components.Num() > 0)
	{
		for (auto& Comp : Components) {
			if (Comp->GetName() == "HoldLocation")
			{
				HoldingComp = Cast<USceneComponent>(Comp);
			}
			else if (Comp->GetName() == "Pickup")
			{
				PickupComp = Cast<UPickupComponent>(Comp);
			}
		}
	}
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHolding && HoldingComp)
	{
		SetActorLocationAndRotation(HoldingComp->GetComponentLocation(), HoldingComp->GetComponentRotation());
	}
}

void APickup::RotateActor()
{
	ControlRotation = GetWorld()->GetFirstPlayerController()->GetControlRotation();
	SetActorRotation(FQuat(ControlRotation));
	//GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Purple, ControlRotation.ToString());
}

// Drop or pick up object
bool APickup::Interact(int DropForce)
{
	bHolding = !bHolding;
	bGravity = !bGravity;

	MyMesh->SetEnableGravity(bGravity);
	MyMesh->SetSimulatePhysics(bHolding ? false : true);
	MyMesh->SetCollisionEnabled(bHolding ? ECollisionEnabled::PhysicsOnly : ECollisionEnabled::QueryAndPhysics);
	
	if (bHolding && HoldingComp)
	{
		MyMesh->AttachToComponent(HoldingComp, FAttachmentTransformRules::KeepWorldTransform);
		
		SetActorLocation(HoldingComp->GetComponentLocation());
		return true;
	}

	if (!bHolding)
	{
		MyMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		ForwardVector = PlayerCamera->GetForwardVector();

		/*GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2.f, FColor::Purple,
					FString::Printf(TEXT("Distance is %s"), ForwardVector.ToString()));*/
		MyMesh->AddForce(ForwardVector * DropForce * MyMesh->GetMass());
	}
	return false;
}

void APickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(FName(TEXT("Glitchable"))))
	{
		bDropped = true;
		Interact(CollisionDropForce);
		return;
	}

	if (bHolding)
	{
		if (PickupComp)
		{
			/*PickupComp->LerpHoldingDistance(HoldDistance);
			bDropped = true;
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.f, FColor::Purple,
				OtherActor->GetName());*/
		}
		// Drop this like its hot
		//IsPickedup(CollisionDropForce);
	}
}

void APickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}


