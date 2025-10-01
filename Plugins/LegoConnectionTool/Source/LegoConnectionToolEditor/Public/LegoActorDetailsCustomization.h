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
	void OnSizeChanged(float NewValue) const;

	TWeakObjectPtr<ALegoActor> SelectedLegoActor;
};