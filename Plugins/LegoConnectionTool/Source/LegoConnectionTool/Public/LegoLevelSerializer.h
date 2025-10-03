// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "EShapeTypes.h"
#include "UObject/Object.h"
#include "LegoLevelSerializer.generated.h"

class ALegoActor;

USTRUCT()
struct FLegoActorImage
{
	GENERATED_BODY()

	UPROPERTY()
	FGuid Guid;

	UPROPERTY()
	FTransform Transform;

	UPROPERTY()
	FLinearColor Color = FLinearColor::White;

	UPROPERTY()
	EShapeTypes Shape = EShapeTypes::Box;

	UPROPERTY()
	float Size = 10.0f;

	UPROPERTY()
	TArray<FGuid> ConnectedGuids;
};

UCLASS()
class LEGOCONNECTIONTOOL_API ULegoLevelSerializer : public UObject
{
	GENERATED_BODY()
	
public:
	static void Save(ALegoActor* Actor, FLegoActorImage& OutImage);
	static bool Load(UWorld* World, const TArray<FLegoActorImage>& Images);
	static bool SerializeLevel(UWorld* World, FString& OutJSON, const FString& FilePath = TEXT(""));
	static bool DeserializeLevel(UWorld* World, const FString& InJSON);
private:

	
};
