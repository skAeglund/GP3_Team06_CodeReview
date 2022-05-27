#pragma once
#include "Components/ActorComponent.h"
#include  "PlayerPawnMovementComponent.generated.h"

UCLASS(Meta = (BlueprintSpawnableComponent))
class UPlayerPawnMovementComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

	UPlayerPawnMovementComponent();

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void PlayerVelocity(float DeltaTime);
	void Jump();
	void CameraControlls();
	void CollisionCheck(float DeltaTime);
	void Gravity();
	void AddForce(const FVector& Force);
	void AddImpulse(const FVector& Impulse);
	float MaxGravity = 980;
	FVector FVectorMoveTowards(FVector current, FVector target, float maxDistanceDelta);

	bool OnGround();
	bool RequestJump;
	
	UPROPERTY()
	AActor* Owner = GetOwner();

	UPROPERTY(VisibleAnywhere);
	FVector Velocity;
	FVector DesiredVelocityForward;
	FVector DesiredVelocityRight;
	FVector Displacement;
	FVector PlayerInput;
	FVector CameraInput;
	FVector PlayerUpVector;
	FVector PlayerForwardVector;
	
	UPROPERTY(EditAnywhere)
	USceneComponent* PlayerMesh;
	
	//Movement
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float Acceleration = 500.f;
	UPROPERTY(EditAnywhere, Category="Movement")
	float MaxMovementSpeed = 600;
	UPROPERTY(EditAnywhere, Category="Movement")
	float MaxAcceleration = 600;
	UPROPERTY(EditDefaultsOnly, Category = "Driving")
	float RollFriction = 0.8f;
	UPROPERTY(EditAnywhere, Category="Movement")
	float JumpStrength = 500;
	UPROPERTY(EditAnywhere, Category="Movement")
	float AirStrafeForce;
	
	//Collision
	UPROPERTY(EditAnywhere, Category="Collision")
	float GroundCheckRayLenght = 60;
	
	//Gravity
	UPROPERTY(EditAnywhere, Category="Gravity")
	FVector GravityForce = FVector(0, 0, -980.0f);
	
	void RotateTowardsGravity();
	void DrawDebugs();
};

