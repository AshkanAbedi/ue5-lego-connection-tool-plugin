#pragma once
#include "CoreMinimal.h"

UENUM()
enum class EShapeTypes : uint8
{
	Box		UMETA(DisplayName = "Box"),
	Sphere	UMETA(DisplayName = "Sphere"),
	Capsule	UMETA(DisplayName = "Capsule"),
	Convex	UMETA(DisplayName = "Convex"),
};