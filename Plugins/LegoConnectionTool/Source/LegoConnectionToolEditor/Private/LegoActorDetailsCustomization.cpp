#include "LegoActorDetailsCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "LegoActor.h"
#include "Editor.h"
#include "Selection.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widgets/Input/SSlider.h"
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
		return; // For now, let's only apply to single selection...
	}

	SelectedLegoActor = Cast<ALegoActor>(SelectedObjects[0].Get());

	if (!SelectedLegoActor.IsValid())
	{
		return;
	}

//------------------------------------------
//Replacing the "size" setting of ALegoActor with a slider to be more designer-friendly
	
	IDetailCategoryBuilder& Category = DetailLayout.EditCategory("Lego Settings");
	TSharedRef<IPropertyHandle> SizeHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(ALegoActor, Size));

	Category.AddProperty(SizeHandle)  // the order is not correct; why? TODO: Checking this
	.CustomWidget()
	.NameContent()
	[
		SizeHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.HAlign(HAlign_Fill)
	[
		SNew(SSlider)
		.MinValue(1.f)
		.MaxValue(100.f)
		.Value_Lambda([SizeHandle]()->float {
			float Val = 0.f;
			SizeHandle->GetValue(Val);
			return Val;
		})
		.OnValueChanged_Lambda([SizeHandle](float NewVal) {
			SizeHandle->SetValue(NewVal);
		})
	];
	
//------------------------------------------
	
//------------------------------------------
//Replacing the "Shape" setting of ALegoActor with 4 buttons representing the shapes.

	TSharedRef<IPropertyHandle> ShapeHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(ALegoActor, Shape));
	Category.AddProperty(ShapeHandle).CustomWidget()
	.NameContent()
	[
		ShapeHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	.MinDesiredWidth(200.f)
	.MaxDesiredWidth(400.f)
	[
		SNew(SHorizontalBox)

		// A → Box
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2,0)
		[
			SNew(SCheckBox)
			.Style(FCoreStyle::Get(), "ToggleButtonCheckbox")
			.OnCheckStateChanged_Lambda([ShapeHandle](ECheckBoxState State) {
            if (State == ECheckBoxState::Checked)
            {
                ShapeHandle->SetValue((uint8)EShapeType::Box);
            }
			})
			.IsChecked_Lambda([ShapeHandle]() -> ECheckBoxState {
            uint8 EnumVal = 0;
            ShapeHandle->GetValue(EnumVal);
            return (EnumVal == (uint8)EShapeType::Box) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
			})
			[
				SNew(SImage).Image(FSlateStyleRegistry::FindSlateStyle("LegoConnectionToolStyle")->GetBrush("LegoConnectionTool.Shape.Box"))
			]
		]

		// B → Sphere
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2,0)
		[
			SNew(SCheckBox)
			.Style(FCoreStyle::Get(), "ToggleButtonCheckbox")
			.OnCheckStateChanged_Lambda([ShapeHandle](ECheckBoxState State) {
            if (State == ECheckBoxState::Checked)
            {
                ShapeHandle->SetValue((uint8)EShapeType::Sphere);
            }
			})
			.IsChecked_Lambda([ShapeHandle]() -> ECheckBoxState {
            uint8 EnumVal = 0;
            ShapeHandle->GetValue(EnumVal);
            return (EnumVal == (uint8)EShapeType::Sphere) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
			})
			[
				SNew(SImage).Image(FSlateStyleRegistry::FindSlateStyle("LegoConnectionToolStyle")->GetBrush("LegoConnectionTool.Shape.Sphere"))
			]
		]

    // C → Capsule
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2,0)
		[
			SNew(SCheckBox)
			.Style(FCoreStyle::Get(), "ToggleButtonCheckbox")
			.OnCheckStateChanged_Lambda([ShapeHandle](ECheckBoxState State) {
            if (State == ECheckBoxState::Checked)
            {
                ShapeHandle->SetValue((uint8)EShapeType::Capsule);
            }
			})
			.IsChecked_Lambda([ShapeHandle]() -> ECheckBoxState {
            uint8 EnumVal = 0;
            ShapeHandle->GetValue(EnumVal);
            return (EnumVal == (uint8)EShapeType::Capsule) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
			})
			[	
				SNew(SImage).Image(FSlateStyleRegistry::FindSlateStyle("LegoConnectionToolStyle")->GetBrush("LegoConnectionTool.Shape.Capsule"))
			]
		]

    // D → Convex
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.Padding(2,0)
		[
			SNew(SCheckBox)
			.Style(FCoreStyle::Get(), "ToggleButtonCheckbox")
			.OnCheckStateChanged_Lambda([ShapeHandle](ECheckBoxState State) {
            if (State == ECheckBoxState::Checked)
            {
                ShapeHandle->SetValue((uint8)EShapeType::Convex);
            }
			})
			.IsChecked_Lambda([ShapeHandle]() -> ECheckBoxState {
            uint8 EnumVal = 0;
            ShapeHandle->GetValue(EnumVal);
            return (EnumVal == (uint8)EShapeType::Convex) ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
			})
			[
				SNew(SImage).Image(FSlateStyleRegistry::FindSlateStyle("LegoConnectionToolStyle")->GetBrush("LegoConnectionTool.Shape.Cone"))
			]
		]
	];
//------------------------------------------	
}

void FLegoActorDetailsCustomization::OnSizeChanged(float NewValue) const
{
	if (SelectedLegoActor.IsValid()){
	
		SelectedLegoActor->Size = NewValue;
		SelectedLegoActor->UpdateAllConnectionData();
	}
}

/*FReply FLegoActorDetailsCustomization::OnAddConnectionClicked()
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
}*/

/*FReply FLegoActorDetailsCustomization::OnRemoveConnectionClicked(ALegoActor* ActorToRemove)
{
	if (SelectedLegoActor.IsValid() && ActorToRemove)
	{
		SelectedLegoActor->RemoveConnection(ActorToRemove);
	}
	return FReply::Handled();
}*/

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


