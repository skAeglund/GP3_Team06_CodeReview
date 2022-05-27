// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GlitchComponent.generated.h"

class UCharacterMovementComponent;
class USpringArmComponent;
class UCapsuleComponent;
class UCameraComponent;

UENUM(BlueprintType)
enum GlitchState
{
	Inactive,
	Stationary,
	Upward,
	Forwards,
	TowardTarget,
	GlitchThrough
};

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3_2022_TEAM06_API UGlitchComponent : public UActorComponent
{
	GENERATED_BODY()


public:	
	// Sets default values for this component's properties
	UGlitchComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void StopGlitching();
	UFUNCTION(BlueprintCallable)
	void StartGlitching(); 
	UFUNCTION(BlueprintCallable)
	float SetGlitchDirection(int GlitchDirection, FVector TargetLocation);
	UFUNCTION(BlueprintCallable)
	void StartShortDash(int GlitchDirection);

	UFUNCTION(BlueprintCallable)
	void ShortenGlitchLength();

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void OnGlitchingStart();
	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void OnGlitchingEnd();
	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void OnDashStart();
	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void OnPassingThroughObject();

private:
	void StaticGlitchUpdate();
	void GlitchThroughUpdate();
	void TargetGlitchUpdate();
	void DashUpdate();
	void PauseTimers();

	UFUNCTION()
	void HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, int32 BodyIndex, bool bFromSweep, const FHitResult& SweepHit);
	UFUNCTION()
	void HandleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooldown")
	bool bEnableCooldown = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooldown")
	bool bIsOnCooldown = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooldown")
	float CooldownDuration = 3.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cooldown")
	float TimeLeftOnCooldown = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<GlitchState> CurrentGlitchState = Inactive;

protected:
	UPROPERTY(BlueprintReadOnly)
		bool bIsGlitching;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Short Dash")
		float DashSpeed = 2500;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Short Dash")
		float DashUpwardSpeed = 2500;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, ClampMax = 0.8f, UIMin = 0.f, UIMax = 0.8f), Category = "Short Dash")
		float DashDuration = 0.15f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Glitch Through Wall")
		float GlitchThroughSpeed = 12000;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Glitch Through Wall")
		float GlitchThroughDuration = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, ClampMax = 0.8f, UIMin = 0.f, UIMax = 0.8f))
		float TargetDashDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Static Glitch")
		float StaticGlitchDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, ClampMax = 10.f, UIMin = 0.f, UIMax = 10.f), Category = "Static Glitch")
		float StaticVibrationStrength = 3;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = 0.00833f, ClampMax = 0.025f, UIMin = 0.00833f, UIMax = 0.025f), Category = "Static Glitch")
		float StaticVibrationInterval = 0.00833f;


	UPROPERTY(BlueprintReadOnly)
	FVector GlitchTargetLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite)
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(BlueprintReadWrite)
	USkeletalMeshComponent* PlayerMesh;

	UPROPERTY(BlueprintReadOnly)
	FVector CurrentGlitchDirection;

	UPROPERTY(BlueprintReadOnly)
	UCapsuleComponent* Collider;

	UPROPERTY(BlueprintReadOnly)
	USceneComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* TargetMovementCurve;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* GlitchThroughCurve;

private:
	UPROPERTY()
	AActor* Owner;
	UCharacterMovementComponent* MoveComp;
	FTimerHandle StaticGlitchTimer;
	FTimerHandle GlitchThroughTimer;
	FTimerHandle TargetGlitchTimer;
	FTimerHandle DashTimer;
	FVector InitialMeshLocalPos;
	FVector PositionBeforeDashing;
	
	float ElapsedGlitchTime = 0.f;
	float ElapsedDashTime = 0.f;
	float ElapsedStaticTime = 0.f;
	float ElapsedAirTime = 0.f;
	float StaticProgress = 0.f;
	float DashProgress = 0.f;
	float SpeedMultiplier = 1.f;
	float DurationMultiplier = 1.f;
	float TransitionTimer = 0.f;
	float VelocityMagnitudeBeforeGlitching = 0.f;
	float MovingGlitchTickRate = 0.00833f;

	//float InitialSpringArmLength;
	//float InitialCameraLag;
};

