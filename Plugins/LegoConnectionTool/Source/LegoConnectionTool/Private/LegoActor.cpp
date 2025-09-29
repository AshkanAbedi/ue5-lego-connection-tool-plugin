// Fill out your copyright notice in the Description page of Project Settings.
#include "LegoActor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ALegoActor::ALegoActor()
{
	PrimaryActorTick.bCanEverTick = false; // Should it tick or not? maybe for updating its data in run-time?! Let's put false for now...I don't think it should tick.
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;

	// TODO: Maybe add a default shape here to be more designer-friendly-->DONE

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CapsuleMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ConvexMesh(TEXT("/Engine/BasicShapes/Cone.Cone"));

	if (BoxMesh.Succeeded()) CachedBox = BoxMesh.Object;
	if (BoxMesh.Succeeded()) CachedSphere = SphereMesh.Object;
	if (BoxMesh.Succeeded()) CachedCapsule = CapsuleMesh.Object;
	if (BoxMesh.Succeeded()) CachedConvex = ConvexMesh.Object;

	StaticMeshComponent->SetStaticMesh(CachedBox);


	// TODO: And also add a function of changing the color and size of the default shape in the editor
}

void ALegoActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	AssignGuid();
}

void ALegoActor::AssignGuid()
{
	if (!UniqueActorGuid.IsValid())
	{
		UniqueActorGuid = FGuid::NewGuid();
	}
}

void ALegoActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	const FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None; 
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ALegoActor, Size)) // if the size property changes, then the bounding sphere will change as well;
								                                   // So I think we need to update the data here. For both the actor and the connected actor to it.
	{
		UpdateAllConnectionData();
		for (const FConnectionData& Connection : Connections)
		{
			if (Connection.ConnectedActor)
			{
				Connection.ConnectedActor->UpdateAllConnectionData();
			}
		}
	}
}

void ALegoActor::PostEditMove(bool bFinished)
{
	Super::PostEditMove(bFinished);
	if (bFinished)
	{
		UpdateAllConnectionData();
		for (const FConnectionData& Connection : Connections) // Same reason as PostEditChangeProperty applies here I think.
		{
			if (Connection.ConnectedActor)
			{
				Connection.ConnectedActor->UpdateAllConnectionData();
			}
		}
	}
}

void ALegoActor::PostLoad()
{
	Super::PostLoad();
	AssignGuid(); // Need a test here;
	//TODO: add a log for testing.
}

void ALegoActor::AddConnection(ALegoActor* OtherActor)
{
	if (!OtherActor || OtherActor == this || GetLevel() != OtherActor->GetLevel())
	{
		return; // Based on the task requirement, the connected actor should be unique; it can't be the actor itself, it can't be null, and it should be on the same level. 
	}

	if (!IsConnectedTo(OtherActor)) // For checking if the other actor is not already connected
	{
		FConnectionData NewConnection;
		NewConnection.ConnectedActor = OtherActor;
		Connections.Add(NewConnection);
		UpdateConnectionData(Connections.Last());
	}

	if (!OtherActor->IsConnectedTo(this)) // for bidirectionality
	{
		OtherActor->AddConnection(this);
	}
}

void ALegoActor::RemoveConnection(ALegoActor* OtherActor)
{
	if (!OtherActor) return;
	
	Connections.RemoveAll([OtherActor](const FConnectionData& Connection){
		return Connection.ConnectedActor == OtherActor; // Cool new thing that I've learned...This function will go through the array and remove the one that we specify using lambda here.
	});
	
	if (OtherActor->IsConnectedTo(this))
	{
		OtherActor->RemoveConnection(this);
	}
}

bool ALegoActor::IsConnectedTo(const ALegoActor* OtherActor)
{
	if (!OtherActor) return false;
	return Connections.Contains(OtherActor);
}

void ALegoActor::UpdateAllConnectionData()
{
	for (FConnectionData& ConnectionData : Connections)
	{
		UpdateConnectionData(ConnectionData);
	}
}

void ALegoActor::UpdateConnectionData(FConnectionData& ConnectionData)
{
	ALegoActor* OtherActor = ConnectionData.ConnectedActor;
	if (!OtherActor || !GetWorld()) return;

	const FVector Start = GetActorLocation();
	const FVector End = OtherActor->GetActorLocation();

	// Line of sight test
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	CollisionParams.AddIgnoredActor(OtherActor);
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
	ConnectionData.bHasLOS = !HitResult.bBlockingHit;

	// Closes point on Bounding Sphere
	const FVector DirectionToOther = (End - Start).GetSafeNormal();
	ConnectionData.ClosestPointOnThisActor = Start + DirectionToOther * (this->Size / 2.0f);

	// Forward direction angle difference
	const FVector ThisActorForward = GetActorForwardVector();
	const FVector OtherActorForward = OtherActor->GetActorForwardVector();
	const float DotProduct = FVector::DotProduct(ThisActorForward, OtherActorForward);
	ConnectionData.ForwardAngleDifference = FMath::Acos(DotProduct); // Radians
	ConnectionData.ForwardAngleDifference = FMath::RadiansToDegrees(ConnectionData.ForwardAngleDifference);
}

void ALegoActor::BeginPlay()
{
	Super::BeginPlay();
}

void ALegoActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


