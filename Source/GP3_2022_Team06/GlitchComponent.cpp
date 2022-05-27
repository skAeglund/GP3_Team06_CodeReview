// Fill out your copyright notice in the Description page of Project Settings.

#include "GlitchComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

class UCameraComponent;
int StaticCounter = 0;

UGlitchComponent::UGlitchComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGlitchComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();
	MoveComp = Cast<ACharacter>(Owner)->GetCharacterMovement();
	Owner->GetWorldTimerManager().SetTimer(StaticGlitchTimer, this, &UGlitchComponent::StaticGlitchUpdate, StaticVibrationInterval, true, 0.f);
	Owner->GetWorldTimerManager().SetTimer(DashTimer, this, &UGlitchComponent::DashUpdate, MovingGlitchTickRate, true, 0.f);
	Owner->GetWorldTimerManager().SetTimer(TargetGlitchTimer, this, &UGlitchComponent::TargetGlitchUpdate, MovingGlitchTickRate, true, 0.f);
	Owner->GetWorldTimerManager().SetTimer(GlitchThroughTimer, this, &UGlitchComponent::GlitchThroughUpdate, MovingGlitchTickRate, true, 0.f);
	PauseTimers();

	Collider = Cast<UCapsuleComponent>(Owner->GetComponentByClass(UCapsuleComponent::StaticClass()));
	CameraComponent = Cast<USceneComponent>(Owner->GetComponentByClass(UCameraComponent::StaticClass()));

	Collider->OnComponentBeginOverlap.AddDynamic(this, &UGlitchComponent::HandleBeginOverlap);
	Collider->OnComponentEndOverlap.AddDynamic(this, &UGlitchComponent::HandleEndOverlap);

	if (CameraComponent == nullptr || Collider == nullptr || PlayerMesh == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10, FColor::Red, TEXT(
			"ERROR: Actor with glitch component needs: Skeletal Mesh, Camera Component, Capsule Collider"));
		DestroyComponent();
		return;
	}
	InitialMeshLocalPos = PlayerMesh->GetRelativeLocation();
}

void UGlitchComponent::StopGlitching()
{
	if (!bIsGlitching) return;
	OnGlitchingEnd();
	bIsGlitching = false;
	CurrentGlitchState = Inactive;
	ElapsedGlitchTime = 0;
	StaticProgress = 0;
	DashProgress = 0;
	TransitionTimer = 1;
	GlitchTargetLocation = FVector::ZeroVector;
	PauseTimers();
	PlayerMesh->SetRelativeLocation(InitialMeshLocalPos);

	float NewVelocityMagnitude = Owner->GetInputAxisValue(TEXT("MoveForward")) == 0 ? 150.f : 850.f;

	MoveComp->Velocity = (FVector(CurrentGlitchDirection.X, CurrentGlitchDirection.Y, 0.f)
		+ FVector::DownVector * 0.25f) * NewVelocityMagnitude;
	
	MoveComp->SetComponentTickEnabled(true);
	Collider->SetCollisionProfileName(TEXT("Pawn"));
	CurrentGlitchDirection = FVector::ZeroVector;
	Owner->CustomTimeDilation = 1.f;
}

void UGlitchComponent::StartGlitching()
{
	OnGlitchingStart();
	CurrentGlitchState = Stationary;
	bIsGlitching = true;
	SpeedMultiplier = 1.f;
	ElapsedDashTime = 0.f;
	ElapsedStaticTime = 0.f;
	Owner->GetWorldTimerManager().UnPauseTimer(StaticGlitchTimer);

	VelocityMagnitudeBeforeGlitching = FMath::Clamp(MoveComp->Velocity.Size(), 0.f, 700.f);
	Owner->CustomTimeDilation = 0.05f;
}

float UGlitchComponent::SetGlitchDirection(int GlitchDirection, FVector TargetLocation = FVector::ZeroVector)
{
	float FXDelay = 0.f;
	Owner->CustomTimeDilation = 1.f;
	if (!bIsGlitching) return FXDelay;
	
	if (bEnableCooldown)
	{
		bIsOnCooldown = true;
		TimeLeftOnCooldown = CooldownDuration;
	}

	CurrentGlitchState = static_cast<GlitchState>(GlitchDirection);
	Collider->SetCollisionProfileName(TEXT("PlayerGlitching"));
	Owner->GetWorldTimerManager().PauseTimer(StaticGlitchTimer);
	if (CurrentGlitchState == TowardTarget)
	{
		float DistanceToTarget = (TargetLocation - Owner->GetActorLocation()).Size();
		float MaxDistance = 3000.f;
		float MinDistance = 500.f;
		float T = FMath::Clamp((DistanceToTarget - MinDistance) / (MaxDistance - MinDistance),0.f, 1.f);
		DurationMultiplier = FMath::Lerp(1.f, 1.8f, T);
		PositionBeforeDashing = Owner->GetActorLocation();
		GlitchTargetLocation = TargetLocation;
		Owner->GetWorldTimerManager().UnPauseTimer(TargetGlitchTimer);
		FXDelay = FMath::Lerp(0.f, 1.6f, T);
	}
	else if (CurrentGlitchState == GlitchThrough)
	{
		Owner->GetWorldTimerManager().UnPauseTimer(GlitchThroughTimer);
		MoveComp->SetComponentTickEnabled(false);
	}

	PlayerMesh->SetRelativeLocation(InitialMeshLocalPos);
	OnDashStart();
	return FXDelay;
}

void UGlitchComponent::StartShortDash(int GlitchDirection)
{
	if (ElapsedAirTime > 0.3f && static_cast<GlitchState>(GlitchDirection) == Upward)
	{
		return;
	}
	OnGlitchingStart();
	Owner->CustomTimeDilation = 1.f;
	if (bEnableCooldown)
	{
		bIsOnCooldown = true;
		TimeLeftOnCooldown = CooldownDuration;
	}

	bIsGlitching = true;
	CurrentGlitchState = static_cast<GlitchState>(GlitchDirection);
	DurationMultiplier = CurrentGlitchState == Forwards ? 1.f : 0.7f;

	VelocityMagnitudeBeforeGlitching = FMath::Clamp(Cast<ACharacter>(Owner)->GetCharacterMovement()->Velocity.Size(), 0.f, 700.f);
	MoveComp->SetComponentTickEnabled(false);
	Owner->GetWorldTimerManager().UnPauseTimer(DashTimer);
	ElapsedDashTime = 0.f;
	SpeedMultiplier = 1.f;
	OnDashStart();
}

void UGlitchComponent::ShortenGlitchLength()
{
	if(bIsGlitching)
	{
		DurationMultiplier = FMath::Lerp(0.8f, 0.95f, StaticProgress);
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 2, FColor::Red, TEXT("Shortened glitch length!"));
	}
}

void UGlitchComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (bIsGlitching)
	{
		ElapsedGlitchTime += DeltaTime;
		if (CurrentGlitchState == Stationary)
		{

			ElapsedStaticTime += DeltaTime;
		}
		else
		{
			ElapsedDashTime += DeltaTime;
		}
	}
	else if (TransitionTimer > 0.f)
	{
		TransitionTimer = FMath::Clamp(TransitionTimer - DeltaTime, 0.f,1.f);
		float TransitionValue = FMath::Pow(TransitionTimer, 3); // cubic ease in
		DynamicMaterial->SetScalarParameterValue(FName(TEXT("TransitionTime")), TransitionValue);
	}

	if (bIsOnCooldown)
	{
		TimeLeftOnCooldown -= DeltaTime;
		if (TimeLeftOnCooldown <= 0.f)
		{
			bIsOnCooldown = false;
			TimeLeftOnCooldown = 0;
		}
	}
	if (!MoveComp->IsMovingOnGround())
	{
		ElapsedAirTime += DeltaTime;
	}
	else if (ElapsedAirTime != 0.f)
	{
		ElapsedAirTime = 0.f;
	}
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGlitchComponent::StaticGlitchUpdate()
{
	// glitch mode - static vibrations
	if (!bIsGlitching)
	{
		StopGlitching();
		return;
	}
	StaticCounter = StaticCounter++ % 8;
	StaticProgress = FMath::Clamp(ElapsedStaticTime / StaticGlitchDuration, 0.f, 1.f);
	

	// vibration directions
	FVector OffsetVector =
		StaticCounter == 1 ? PlayerMesh->GetRightVector() :
		StaticCounter == 2 ? PlayerMesh->GetForwardVector() * 2 :
		StaticCounter == 3 ? PlayerMesh->GetRightVector() * -1:
		StaticCounter == 4 ? PlayerMesh->GetForwardVector() * -1 :
		StaticCounter == 5 ? PlayerMesh->GetRightVector() * 2  :
		StaticCounter == 6 ? PlayerMesh->GetForwardVector() :
		StaticCounter == 7 ? PlayerMesh->GetRightVector() * -2 :
							 PlayerMesh->GetForwardVector() * -2;

	PlayerMesh->AddWorldOffset(OffsetVector * StaticVibrationStrength);
}
void UGlitchComponent::GlitchThroughUpdate()
{
	if (!bIsGlitching || ElapsedDashTime > (GlitchThroughDuration * DurationMultiplier))
	{
		StopGlitching();
		return;
	}
	DashProgress = ElapsedDashTime / (GlitchThroughDuration * DurationMultiplier);

	CurrentGlitchDirection = CurrentGlitchState == Upward ? FVector::UpVector : CameraComponent->GetForwardVector();

	FVector RandomVector = FMath::VRand() * FMath::RandRange(0.f, 1.f);
	float EasedAlpha = FMath::Pow(DashProgress, 4);

	FVector OffsetVector = CurrentGlitchDirection * GlitchThroughSpeed * SpeedMultiplier * EasedAlpha + RandomVector;
	FHitResult Hit;
	float RemainingTime = MovingGlitchTickRate;
	// add movement
	while (RemainingTime > 0.f)
	{
		Owner->AddActorWorldOffset(OffsetVector * RemainingTime, true, &Hit);
		if (Hit.bBlockingHit)
		{
			if (Hit.bStartPenetrating)
			{
				// de-penetrate
				Owner->AddActorWorldOffset(Hit.Normal * (Hit.PenetrationDepth + 0.1f));
			}
			else
			{
				// slide alongside collision
				OffsetVector = FVector::VectorPlaneProject(OffsetVector, Hit.Normal);
				RemainingTime -= RemainingTime * Hit.Time;
			}
		}
		else
		{
			break;
		}
	}
}
void UGlitchComponent::DashUpdate()
{
	if (!bIsGlitching || ElapsedDashTime > (DashDuration *DurationMultiplier))
	{
		StopGlitching();
		return;
	}
	DashProgress = ElapsedDashTime / (DashDuration *DurationMultiplier);

	if (CurrentGlitchState == Upward)
	{
		CurrentGlitchDirection = FVector::UpVector;
	}
	else 
	{
		CurrentGlitchDirection = FVector(CameraComponent->GetForwardVector().X, CameraComponent->GetForwardVector().Y, 0.f);
		CurrentGlitchDirection.Normalize();
	}
	FVector RandomVector = FMath::VRand() * FMath::RandRange(0.f,1.f);
	float EasedAlpha = 1 - FMath::Pow(1 - DashProgress, 3);
	
	FVector OffsetVector = CurrentGlitchDirection * (CurrentGlitchState == Upward ? DashUpwardSpeed : DashSpeed)
							* SpeedMultiplier * EasedAlpha + RandomVector;
	FHitResult Hit;
	float RemainingTime = MovingGlitchTickRate;
	while (RemainingTime > 0.f)
	{
		Owner->AddActorWorldOffset(OffsetVector * RemainingTime, true, &Hit);
		if (Hit.bBlockingHit)
		{
			if (Hit.bStartPenetrating)
			{
				Owner->AddActorWorldOffset(Hit.Normal * (Hit.PenetrationDepth + 0.1f));
			}
			else
			{
				OffsetVector = FVector::VectorPlaneProject(OffsetVector, Hit.Normal);
				RemainingTime -= RemainingTime * Hit.Time;
			}
		}
		else
		{
			break;
		}
	}
}
float EaseInOutQuart(float x)
{
	return x < 0.5 ? 8 * x * x * x * x : 1 - FMath::Pow(-2 * x + 2, 4) / 2;
}
float EaseInCirc(float x)
{
	return 1 - FMath::Sqrt(1 - FMath::Pow(x, 2));
}
float EaseInBack(float x)
{
	//float c1 = 1.70158f;
	float c1 = 0.6f;
	float c3 = c1 + 1;
	return c3 * x * x * x - c1 * x * x;
}

void UGlitchComponent::TargetGlitchUpdate()
{
	if (!bIsGlitching || ElapsedDashTime > (TargetDashDuration * DurationMultiplier))
	{
		StopGlitching();
		return;
	}

	DashProgress = ElapsedDashTime / (TargetDashDuration * DurationMultiplier);
	float EasedProgress = 0;
	if (CurrentGlitchState == TowardTarget)
	{
		if (TargetMovementCurve->IsValidLowLevel())
			EasedProgress = TargetMovementCurve->GetFloatValue(DashProgress);
	}
	else
	{
		if (GlitchThroughCurve->IsValidLowLevel())
			EasedProgress = GlitchThroughCurve->GetFloatValue(DashProgress);
	}
	FVector NewPosition = FMath::Lerp(PositionBeforeDashing, GlitchTargetLocation, EasedProgress);
	FHitResult Hit;
	Owner->SetActorLocation(NewPosition, false, &Hit);

}

void UGlitchComponent::HandleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComponent, int32 BodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	// TODO: OnStartingPassingThroughObject() for VFX maybe
}

void UGlitchComponent::HandleEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherComp->K2_IsPhysicsCollisionEnabled() && bIsGlitching && CurrentGlitchState != Stationary && ElapsedDashTime > 0.1f)
	{
		OnPassingThroughObject();
		SpeedMultiplier = 0.1f;
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1, FColor::Black, TEXT("Inside End Overlap"));
	}
}
void UGlitchComponent::PauseTimers()
{
	Owner->GetWorldTimerManager().PauseTimer(StaticGlitchTimer);
	Owner->GetWorldTimerManager().PauseTimer(DashTimer);
	Owner->GetWorldTimerManager().PauseTimer(TargetGlitchTimer);
	Owner->GetWorldTimerManager().PauseTimer(GlitchThroughTimer);
}

void UGlitchComponent::OnGlitchingEnd_Implementation()
{
	// BP event
}
void UGlitchComponent::OnGlitchingStart_Implementation()
{
	// BP event
}
void UGlitchComponent::OnDashStart_Implementation()
{
	// BP event
}

void UGlitchComponent::OnPassingThroughObject_Implementation()
{
	// BP event
}