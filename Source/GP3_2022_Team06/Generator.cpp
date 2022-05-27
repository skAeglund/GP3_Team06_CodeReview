// Fill out your copyright notice in the Description page of Project Settings.


#include "Generator.h"

#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Pickup/PickupComponent.h"


// Sets default values
AGenerator::AGenerator()
{
	GeneratorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GenMesh"));
	TextBox = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text>Box"));
	TextBox->SetVisibility(false);
	TextBox->SetupAttachment(RootComponent);
	InteractArea = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	InteractArea->SetBoxExtent(FVector(128, 128, 64));
	InteractArea->SetVisibility(true);
}

// Called when the game starts or when spawned
void AGenerator::BeginPlay()
{
	Super::BeginPlay();
	TextBox->SetVisibility(false);
	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	KeyConfirmed = false;
	InputComponent = PlayerPawn->InputComponent;

	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AGenerator::KeyDropped);
	}
}

void AGenerator::KeyDropped()
{
	if (KeyConfirmed)
	{
		TextBox->SetText(FText::FromString("Opening door"));
		TextBox->SetVisibility(true);
		EnterGenArea.ExecuteIfBound(SentID);
		if (LastKey != nullptr)
		{
			LastKey->Destroy();
		}
	}
}

void AGenerator::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (OtherActor == PlayerPawn)
	{
		PickupComponent = OtherActor->FindComponentByClass<UPickupComponent>();
		if (KeyActor)
		{
			if (PickupComponent->CurrentItem == nullptr)
			{
				TextBox->SetText(FText::FromString("Bring me a key"));
				TextBox->SetVisibility(true);
			}
			else if (PickupComponent->CurrentItem->GetName() == KeyActor->GetName())
			{
				KeyConfirmed = true;
				TextBox->SetText(FText::FromString("Drop Key to open"));
				LastKey = PickupComponent->CurrentItem;
				TextBox->SetVisibility(true);
			}
			else
			{
				TextBox->SetText(FText::FromString("Wrong key"));
				TextBox->SetVisibility(true);
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("WHOOPS, NullPtr in Generator " + GetName() +
				                                 " Have you entered a keyActor?"));
		}
	}
	else if (OtherActor == KeyActor) // this will sometimes manage to "pick up" keys that are dropped incorrectly
	{
		KeyConfirmed = true;
		LastKey = KeyActor; // This will probably break in a later update
		AGenerator::KeyDropped();
	}
}

void AGenerator::NotifyActorEndOverlap(AActor* OtherActor)
{
	if (OtherActor == PlayerPawn || OtherActor == KeyActor)
	{
		TextBox->SetVisibility(false);
		KeyConfirmed = false;
		LastKey = nullptr;
	}
}
