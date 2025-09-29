#include "LEGOActorDetailsCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "LEGOActor.h"
#include "Editor.h"
#include "Selection.h"
//#include "LegoSerializer.h"

TSharedRef<IDetailCustomization> FLegoActorDetailsCustomization::MakeInstance()
{
	return MakeShareable(new FLegoActorDetailsCustomization);
}

void FLegoActorDetailsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	TArray<TWeakObjectPtr<UObject>> SelectedObjects;
	DetailLayout.GetObjectsBeingCustomized(SelectedObjects);

	if (SelectedObjects.Num() != 1)
	{
		return; // For now, let's only apply single selection...
	}

	SelectedLegoActor = Cast<ALegoActor>(SelectedObjects[0].Get());
	if (!SelectedLegoActor.IsValid())
	{
		return;
	}

	IDetailCategoryBuilder& Category = DetailLayout.EditCategory("Connection Settings", FText::GetEmpty(), ECategoryPriority::Important);

	Category.AddCustomRow(FText::FromString(TEXT("Add Connection")))
	.ValueContent()
	[
		SNew(SButton)
		.Text(FText::FromString("Add Selected Actor as Connection"))
		.OnClicked(this, &FLegoActorDetailsCustomization::OnAddConnectionClicked)
		.ToolTipText(FText::FromString("Select another Lego Actor in the level and click this to connect them."))
	];
	if (SelectedLegoActor->Connections.Num() > 0)
	{
		for (const FConnectionData& Connection : SelectedLegoActor->Connections)
		{
			if (Connection.ConnectedActor)
			{
				ALegoActor* ActorToRemove = Connection.ConnectedActor;
				
				Category.AddCustomRow(FText::FromString(TEXT("Connection")))
				.NameContent()
				[
					SNew(STextBlock).Text(FText::FromString(Connection.ConnectedActor->GetActorLabel()))
				]
				.ValueContent()
				[
					SNew(SButton)
					.Text(FText::FromString("Remove Connection"))
					.OnClicked_Lambda([this, ActorToRemove](){if (SelectedLegoActor.IsValid() && ActorToRemove) { this->OnRemoveConnectionClicked(ActorToRemove);} return FReply::Handled();})
				];
			}
		}
	} else
	{
		Category.AddCustomRow(FText::FromString(TEXT("No Connections")))
		.WholeRowContent()
		[
			SNew(STextBlock).Text(FText::FromString("No actors connected.")).Justification(ETextJustify::Center)
		];
	}

	/*IDetailCategoryBuilder& SerializationCategory = DetailLayout.EditCategory("LEGO Serialization", FText::GetEmpty(), ECategoryPriority::Default);

	SerializationCategory.AddCustomRow(FText::FromString(TEXT("Serialize")))
	.ValueContent()
	[
		SNew(SButton)
		.Text(FText::FromString("Serialize All LEGO Actors"))
		.OnClicked(this, &FLegoActorDetailsCustomization::OnSaveClicked)
	];

	SerializationCategory.AddCustomRow(FText::FromString(TEXT("Deserialize")))
	.ValueContent()
	[
		SNew(SButton)
		.Text(FText::FromString("Deserialize All LEGO Actors"))
		.OnClicked(this, &FLegoActorDetailsCustomization::OnLoadClicked)
	];*/
}

FReply FLegoActorDetailsCustomization::OnAddConnectionClicked()
{
	if (SelectedLegoActor.IsValid())
	{
		// Find the other selected actor in the editor
		USelection* EditorSelection = GEditor->GetSelectedActors();
		TArray<ALegoActor*> SelectedLegoActors;
		EditorSelection->GetSelectedObjects<ALegoActor>(SelectedLegoActors);

		for (ALegoActor* ActorToAdd : SelectedLegoActors)
		{
			// Add connection if it's a valid, different actor
			if (ActorToAdd && ActorToAdd != SelectedLegoActor.Get())
			{
				SelectedLegoActor->AddConnection(ActorToAdd);
			}
		}
	}
	return FReply::Handled();
}


FReply FLegoActorDetailsCustomization::OnRemoveConnectionClicked(ALegoActor* ActorToRemove)
{
	if (SelectedLegoActor.IsValid() && ActorToRemove)
	{
		SelectedLegoActor->RemoveConnection(ActorToRemove);
	}
	return FReply::Handled();
}

/*FReply FLegoActorDetailsCustomization::OnSaveClicked()
{
	if(SelectedLegoActor.IsValid())
	{
		FLegoSerializer::SerializeActors(SelectedLegoActor->GetWorld());
	}
	return FReply::Handled();
}

FReply FLegoActorDetailsCustomization::OnLoadClicked()
{
	if(SelectedLegoActor.IsValid())
	{
		FLegoSerializer::DeserializeActors(SelectedLegoActor->GetWorld());
	}
	return FReply::Handled();
}*/


