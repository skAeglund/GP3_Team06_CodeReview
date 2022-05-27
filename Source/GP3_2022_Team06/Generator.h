// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextRenderActor.h"
#include "GameFramework/Actor.h"
#include "Player/Pickup/PickupComponent.h"
#include "Generator.generated.h"
DECLARE_DELEGATE_OneParam(GeneratorDelegate, int id);
UCLASS()
class GP3_2022_TEAM06_API AGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGenerator();
	GeneratorDelegate EnterGenArea;
	GeneratorDelegate ExitGenArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* InteractArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APickup* KeyActor; // This is needed, otherwise causes issues

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SentID;

protected:
	virtual void BeginPlay() override;
	virtual void KeyDropped();
	virtual void NotifyActorBeginOverlap(AActor* OtherActor);
	virtual void NotifyActorEndOverlap(AActor* OtherActor);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStaticMeshComponent* GeneratorMesh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UTextRenderComponent* TextBox;

	APawn* PlayerPawn;
	bool KeyConfirmed;
	UInputComponent* InputComponent;
	UPickupComponent* PickupComponent;
	APickup* LastKey;
};
