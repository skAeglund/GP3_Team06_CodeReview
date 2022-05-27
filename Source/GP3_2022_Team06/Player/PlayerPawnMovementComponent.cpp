// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawnMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"

UPlayerPawnMovementComponent::UPlayerPawnMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerPawnMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	PlayerVelocity(DeltaTime);
	CameraControlls();
	OnGround();
	//RotateTowardsGravity();
	Jump();
	//DrawDebugs();
}

void UPlayerPawnMovementComponent::Gravity()
{
	if (!OnGround())
	{
		AddForce(GravityForce);
	}
	/*
	if (Velocity.X > GravityForce.X)
	{
		AddForce(GravityForce);
	}
	else if (Velocity.Z > GravityForce.Z)
	{
		AddForce(GravityForce);
	}
	else
	{
		if (Velocity.Y > GravityForce.Y)
		{
			AddForce(GravityForce);
		}
	}*/
}

void UPlayerPawnMovementComponent::RotateTowardsGravity()
{
	const FRotator Rot = FRotationMatrix::MakeFromZX(-GravityForce, Owner->GetActorForwardVector()).Rotator();
	//Owner->AddActorWorldRotation(dotResult, false)
	FVector Forward = GravityForce - Owner->GetActorLocation();
	//LookingActor.SetActorRotation(Rot, true);
	Owner->AddActorWorldRotation(Rot, true);
}

void UPlayerPawnMovementComponent::PlayerVelocity(float DeltaTime)
{
	DesiredVelocityForward =  Owner->GetActorForwardVector() * PlayerInput.X * MaxMovementSpeed;
	DesiredVelocityRight = Owner->GetActorRightVector() * PlayerInput.Y * MaxMovementSpeed;
	
	const float MaxSpeedChange = MaxAcceleration * DeltaTime;
	Velocity = FVectorMoveTowards(Velocity, DesiredVelocityForward + DesiredVelocityRight, MaxSpeedChange);

	//Gravity force depending on gravity direction;
	Gravity();

	//CollitionChecking
	Owner->AddActorWorldOffset(Velocity * DeltaTime, true);
	CollisionCheck(DeltaTime);
}

void UPlayerPawnMovementComponent::Jump()
{
	if (RequestJump)
	{
		AddImpulse(-GravityForce.GetSafeNormal() * JumpStrength);
		RequestJump = false;
	}
}

void UPlayerPawnMovementComponent::CameraControlls()
{
	//Rotate our actor's yaw, which will turn our camera because we're attached to it
	FRotator NewRotation = Owner->GetActorRotation();
	float dotResult = FVector::DotProduct(FVector(0,0,1), Owner->GetActorUpVector());
	
	if (dotResult > 0)
	{
		NewRotation.Yaw += CameraInput.X;
	}
	else
	{
		NewRotation.Yaw -= CameraInput.X;
	}
	
	Owner->SetActorRotation(NewRotation);
}


//TODO slide along surface and use built in.
void UPlayerPawnMovementComponent::CollisionCheck(float DeltaTime)
{
	float RemainingTime = DeltaTime;
	int Iterations = 0;
	
	while (RemainingTime > 0.f && ++Iterations < 10)
	{
		FHitResult Hit;
		Owner->AddActorWorldOffset(Velocity * RemainingTime, true, &Hit);
		
		if (Hit.bBlockingHit)
		{
			if (Hit.bStartPenetrating)
			{
				Owner->AddActorWorldOffset(Hit.Normal * (Hit.PenetrationDepth + 0.1f));
			}
			else
			{
				Velocity = FVector::VectorPlaneProject(Velocity, Hit.Normal);
				RemainingTime -= RemainingTime * Hit.Time;
			}
		}
		else
		{
			break;
		}

	}
}

bool UPlayerPawnMovementComponent::OnGround()
{
	FHitResult OutHit;
	FVector RayStartLocation = Owner->GetActorLocation();
	FVector EndRay = RayStartLocation + GravityForce.GetSafeNormal() * GroundCheckRayLenght;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Owner);
	DrawDebugLine(GetWorld(), RayStartLocation, EndRay, FColor::Green, false, 1, 0, 1);
	bool IsHit = GetWorld()->LineTraceSingleByChannel(OutHit, RayStartLocation, EndRay, ECC_Visibility, CollisionParams);

	if (IsHit)
		return true;
	
	return  false;
}

FVector UPlayerPawnMovementComponent::FVectorMoveTowards(FVector current, FVector target, float maxDistanceDelta)
{
	FVector a = target - current;
	float magnitude = a.Size();
	if (magnitude <= maxDistanceDelta || magnitude == 0.f)
	{
		return target;
	}
	return current + a / magnitude * maxDistanceDelta;
}

void UPlayerPawnMovementComponent::AddForce(const FVector& Force)
{
	Velocity += Force * GetWorld()->GetDeltaSeconds();
}

void UPlayerPawnMovementComponent::AddImpulse(const FVector& Impulse)
{
	Velocity += Impulse;
}

void UPlayerPawnMovementComponent::DrawDebugs()
{
	FVector RayStartLocation = Owner->GetActorLocation();
	FVector EndRay = RayStartLocation + Owner->GetActorForwardVector() * 100;;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(Owner);
	DrawDebugLine(GetWorld(), RayStartLocation, EndRay, FColor::Green, false, 1, 0, 1);
}


