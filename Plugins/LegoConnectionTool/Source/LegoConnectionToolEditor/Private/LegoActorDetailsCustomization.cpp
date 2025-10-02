#include "LegoActorDetailsCustomization.h"
#include "DetailLayoutBuilder.h"
#include "DetailCategoryBuilder.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Editor.h"
#include "EngineUtils.h"
#include "LegoActor.h"
#include "LegoLevelSerializer.h"
#include "Styling/SlateStyleRegistry.h"

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

#pragma region Lego Selection Tool Section
//------------------------------------------
//**Creating a new section in the details panel called 'Lego Connection Tool'
//TODO:Needs heavy refactoring

	IDetailCategoryBuilder& ConnectionCategory = DetailLayout.EditCategory("Lego Connection Tool");
	UWorld* World = SelectedLegoActor->GetWorld();

	// Add Connection Drop-down list
	if (World)
	{
		for (TActorIterator<ALegoActor> It(World); It; ++It)
		{
			ALegoActor* OtherActor = *It;
			if (OtherActor && OtherActor !=  SelectedLegoActor.Get())
			{
				if (SelectedLegoActor->IsConnectedTo(OtherActor))
				{
					continue;
				}
				FString ActorName = OtherActor->GetActorLabel();
				ActorNames.Add(MakeShared<FString>(ActorName));
				NameToActorMap.Add(ActorName, OtherActor);
			}
		}
	}

	static TSharedPtr<FString> CurrentlySelected;
	
	ConnectionCategory.AddCustomRow(FText::FromString("Add Lego Connection"))
	.NameContent()[SNew(STextBlock).Text(FText::FromString("Add Connection"))]
	.ValueContent()
	.MinDesiredWidth(200.f)
	[
		SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&ActorNames)
		.Content()
        [
        	SNew(STextBlock)
        	.Text_Lambda([&]()
        	{
        			   return CurrentlySelected.IsValid() ? FText::FromString(*CurrentlySelected) : FText::FromString("Select a Lego Actor to Connect");
        	})
        ]
		.OnGenerateWidget_Lambda([](const TSharedPtr<FString> InItem)
			{
				return SNew(STextBlock).Text(FText::FromString(*InItem));
			})
		.OnSelectionChanged_Lambda([this, &DetailLayout](const TSharedPtr<FString> NewValue, const ESelectInfo::Type SelectInfo)
			{
				if (NewValue.IsValid() && SelectedLegoActor.IsValid())
				{
					if (TWeakObjectPtr<ALegoActor> Target = NameToActorMap.FindRef(*NewValue); Target.IsValid())
					   {
							SelectedLegoActor->AddConnection(Target.Get());
							DetailLayout.ForceRefreshDetails();
					   }
				}
			})
	];
	
	// Remove Connection Drop-down list
	for (const FConnectionData& Connection : SelectedLegoActor->Connections)
	{
		if (ALegoActor* Connected = Connection.ConnectedActor.Get())
		{
			FString ConnectionName = Connected->GetActorLabel();
			ConnectedActorNames.Add(MakeShared<FString>(ConnectionName));
			ConnectedNameToActorMap.Add(ConnectionName, Connected);
		}
	}
	
	ConnectionCategory.AddCustomRow(FText::FromString("Remove Lego Connection"))
	.NameContent()[SNew(STextBlock).Text(FText::FromString("Remove Connection"))]
	.ValueContent()
	.MinDesiredWidth(200.f)
	[
		SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(&ConnectedActorNames)
		.Content()
		[
			SNew(STextBlock)
			.Text_Lambda([&]()
			{
				return CurrentlySelected.IsValid() ? FText::FromString(*CurrentlySelected) : FText::FromString("Remove a Connected Lego Actor");
			})
		]
		.OnGenerateWidget_Lambda([](const TSharedPtr<FString> InItem)
			{
				return SNew(STextBlock).Text(FText::FromString(*InItem));
			})
		.OnSelectionChanged_Lambda([this, &DetailLayout](const TSharedPtr<FString> NewValue, const ESelectInfo::Type SelectInfo)
			{
				if (NewValue.IsValid() && SelectedLegoActor.IsValid())
				{
					if (TWeakObjectPtr<ALegoActor> Target = ConnectedNameToActorMap.FindRef(*NewValue); Target.IsValid())
					{
						 SelectedLegoActor->RemoveConnection(Target.Get());
						 DetailLayout.ForceRefreshDetails();
					}
			 }
		 })
	];

	// Save Button
	ConnectionCategory.AddCustomRow(FText::FromString("Save Lego Data"))
	.WholeRowContent()
	[
		SNew(SButton)
		.Text(FText::FromString("Save"))
		.OnClicked_Lambda([this]()
		{
			if (SelectedLegoActor.IsValid())
			{
				FString JSON;
				if (ULegoLevelSerializer::SerializeLevel(SelectedLegoActor->GetWorld(), JSON, TEXT("LegoSaveTest.json")))
				{
					UE_LOG(LogTemp, Log, TEXT("Serialized JSON:\n%s"), *JSON);
				}
			}
			return FReply::Handled();
		})
	];

	//Load Button
	ConnectionCategory.AddCustomRow(FText::FromString("Load Lego Data"))
	.WholeRowContent()
	[
		SNew(SButton)
		.Text(FText::FromString("Load"))
		.OnClicked_Lambda([this]()
		{
			if (SelectedLegoActor.IsValid())
			{
				FString JSON;
				FFileHelper::LoadFileToString(JSON, *(FPaths::ProjectSavedDir() / TEXT("LegoSaveTest.json")));
				ULegoLevelSerializer::DeserializeLevel(SelectedLegoActor->GetWorld(), JSON);
			}
			return FReply::Handled();
		})
	];

#pragma endregion

#pragma region ShapeCustomization
//------------------------------------------
//**Replacing the "Shape" setting of ALegoActor with 4 buttons representing the shapes. Much cooler and more designer-friendly:D
//TODO: Definitely needs refactoring

	TSharedRef<IPropertyHandle> ShapeHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(ALegoActor, Shape));
	IDetailCategoryBuilder& Category = DetailLayout.EditCategory("Lego Settings");
	Category.AddProperty(ShapeHandle).CustomWidget()
	.NameContent()[ShapeHandle->CreatePropertyNameWidget()]
	.ValueContent()
	.MinDesiredWidth(200.f)
	.MaxDesiredWidth(400.f)
	[
		SNew(SHorizontalBox)

		// A → Cube
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
				SNew(SImage).Image(FSlateStyleRegistry::FindSlateStyle("LegoConnectionToolStyle")->GetBrush("LegoConnectionTool.Shape.Cube"))
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
#pragma endregion

#pragma region ShapeCustomizationRefactoring
	/*TSharedRef<IPropertyHandle> ShapeHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(ALegoActor, Shape));
	DetailLayout.HideProperty(ShapeHandle);
	IDetailCategoryBuilder& Category = DetailLayout.EditCategory("Lego Settings");

	auto CreateButtonShape = [&](EShapeType TargetShape, const FName& IconName)->TSharedRef<SWidget>
	{
		return SNew(SCheckBox)
		.Style(FCoreStyle::Get(), "ToggleButtonCheckbox")
	}*/
	// Time's up :(
#pragma endregion

#pragma region SizeCustomization
//**------------------------------------------
//Replacing the "size" setting of ALegoActor with a slider to be more designer-friendly
	
	TSharedRef<IPropertyHandle> SizeHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_CHECKED(ALegoActor, Size));
	DetailLayout.HideProperty(SizeHandle);
	Category.AddCustomRow(SizeHandle->GetPropertyDisplayName())
		.NameContent()
		[
			SizeHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SSlider)
			.MinValue(1.f)
			.MaxValue(100.f)
			.Value_Lambda([SizeHandle](){ float V = 0.f; return (SizeHandle->GetValue(V) == FPropertyAccess::Success) ? V : 1.f; })
			.OnValueChanged_Lambda([SizeHandle](float NewV){ SizeHandle->SetValue(NewV); })
		];
//------------------------------------------
#pragma endregion
}

void FLegoActorDetailsCustomization::OnSizeChanged(float NewValue) const
{
	if (SelectedLegoActor.IsValid()){
	
		SelectedLegoActor->Size = NewValue;
		SelectedLegoActor->UpdateAllConnectionData();
	}
}


