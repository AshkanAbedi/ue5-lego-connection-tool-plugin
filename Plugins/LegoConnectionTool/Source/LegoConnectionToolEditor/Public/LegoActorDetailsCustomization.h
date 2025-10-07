#pragma once
#include "CoreMinimal.h"
#include "IDetailCustomization.h"

class IDetailLayoutBuilder;
class ALegoActor;

class FLegoActorDetailsCustomization : public IDetailCustomization
{
public:
	static TSharedRef<IDetailCustomization> MakeInstance();

	// IDetailCustomization interface
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

private:
	TWeakObjectPtr<ALegoActor> SelectedLegoActor;
	TArray<TSharedPtr<FString>> ActorNames;
	TArray<TSharedPtr<FString>> ConnectedActorNames;
	TMap<FString, TWeakObjectPtr<ALegoActor>> NameToActorMap;
	TMap<FString, TWeakObjectPtr<ALegoActor>> ConnectedNameToActorMap;
};