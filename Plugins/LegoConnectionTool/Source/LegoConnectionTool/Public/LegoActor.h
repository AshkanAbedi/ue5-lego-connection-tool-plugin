// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "EShapeTypes.h"
#include "LegoActor.generated.h"

class ALegoActor;
class UStaticMesh;
class UStaticMeshComponent;

USTRUCT(BlueprintType)
struct FConnectionData
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, Category="Connection Settings")
	TWeakObjectPtr<ALegoActor> ConnectedActor = nullptr; // I'm not sure whether weak pointer is better here or a strong pointer?! for now, let's use weak...
	
	UPROPERTY(VisibleAnywhere, Category="Connection Settings")
	bool bHasLOS = false; // for checking line of sight
	
	UPROPERTY(VisibleAnywhere, Category="Connection Settings")
	FVector ClosestPointOnThisActor = FVector::ZeroVector; // for checking the sphere bound of the actor
	
	UPROPERTY(VisibleAnywhere, Category="Connection Settings")
	float ForwardAngleDifference = 0.0f; // for checking the forward angle difference
	
	bool operator==(const ALegoActor* Other) const // I'm overloading the == operator here so that I can easily check 'ConnectedActor' to 'other' later in the code.
	{
		return ConnectedActor == Other; 
	}
};

UCLASS(Blueprintable, BlueprintType)
class LEGOCONNECTIONTOOL_API ALegoActor : public AActor
{
	GENERATED_BODY()

public:
	ALegoActor();
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lego Settings")
	EShapeTypes Shape = EShapeTypes::Box;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lego Settings", Meta=(ClampMin="1.0", ClampMax="100.0", UIMin="1.0", UIMax="100.0"))
	float Size = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lego Settings")
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(VisibleAnywhere, Category="Lego Settings", Meta=(DisplayName="Connections"))
	TArray<FConnectionData> Connections;

	UPROPERTY(VisibleAnywhere, transient, Category="Lego Settings", AdvancedDisplay)
	FGuid UniqueActorGuid; // I'm adding this here for serialization later.

//**-------------------------
//These are for making the actor more designer-friendly to edit; To have a starting shape/size/color;
	UPROPERTY()
	UStaticMesh* CachedBox;

	UPROPERTY()
	UStaticMesh* CachedSphere;
	
	UPROPERTY()
	UStaticMesh* CachedCapsule;
	
	UPROPERTY()
	UStaticMesh* CachedConvex;
//-------------------------

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditMove(bool bFinished) override; // I'm overriding this here because we need the data to be updated if we move the actor in the level.
	virtual void PostLoad() override; // I think this will be necessary as well for deserialization, maybe?! let's see...
	void ChangeShape(); // For changing the shape in editor
	void ChangeColor(); // for changing the color in editor

//**--------------------------
//These are the functions that I need to call from the editor tool. TODO: Maybe I'll add a Macro for it later...
	void AddConnection(ALegoActor* OtherActor);
	void RemoveConnection(ALegoActor* OtherActor);
	bool IsConnectedTo(const ALegoActor* OtherActor);
	void UpdateConnectionData(FConnectionData& ConnectionData);
	void UpdateAllConnectionData();
//--------------------------

protected:
	virtual void BeginPlay() override;

private:
	void AssignGuid();
};
