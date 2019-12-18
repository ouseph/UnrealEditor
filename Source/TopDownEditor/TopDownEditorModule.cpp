// Fill out your copyright notice in the Description page of Project Settings.

#include "TopDownEditorModule.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "ConversationAssetActions.h"
#include "EngineUtils.h"
#include "TopDown/SpawnerActor.h"
#include "HAL/ConsoleManager.h"
#include "TopDownEditorCommands.h"
#include "LevelEditor.h"

IMPLEMENT_MODULE(FTopDownEditorModule, TopDownEditor)

void FTopDownEditorModule::StartupModule() {


	//ADD asset tools module
	FAssetToolsModule& oToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
	IAssetTools& oAssetToolsModule = oToolsModule.Get();


	conversationActions = MakeShareable(new FConversationAssetActions());

	oAssetToolsModule.RegisterAssetTypeActions(conversationActions.ToSharedRef());

	//Register command
	commandName = TEXT("DoSpawn");


	IConsoleManager::Get().RegisterConsoleCommand(commandName, TEXT("USE: DoSpawn actor_name"),
		FConsoleCommandWithWorldAndArgsDelegate::CreateRaw(this, &FTopDownEditorModule::CommandDoSpawn), ECVF_Default);
	
	//toolbar

	FTopDownEditorCommands::Register();

	TSharedPtr<FUICommandList> commandList = MakeShareable(new FUICommandList());

	commandList.Get()->MapAction(FTopDownEditorCommands::Get().conversationWindowCommand,
		FExecuteAction::CreateRaw(this, &FTopDownEditorModule::CommandToolbar));

	extender = MakeShareable(new FExtender());
	

	FToolBarBuilder toolBarBuilder(commandList, FMultiBoxCustomization::None,
		extender);


	extender.Get()->AddToolBarExtension("Content", EExtensionHook::Before, commandList,
		FToolBarExtensionDelegate::CreateLambda([](FToolBarBuilder& toolBarBuilder) {
				toolBarBuilder.AddToolBarButton(FTopDownEditorCommands::Get().conversationWindowCommand);
			}
	));

	FLevelEditorModule& oLevelModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	oLevelModule.GetToolBarExtensibilityManager()->AddExtender(extender);

}

void FTopDownEditorModule::ShutdownModule() {

	if (FModuleManager::Get().IsModuleLoaded("AssetTools")) {

		IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>(
			"AssetTools").Get();

		assetTools.UnregisterAssetTypeActions(conversationActions.ToSharedRef());
	}

	conversationActions.Reset();

	IConsoleManager& consoleManager = IConsoleManager::Get();

	consoleManager.UnregisterConsoleObject(commandName);
}


void  FTopDownEditorModule::CommandDoSpawn(const TArray<FString>& _arrParams, UWorld* _pWorld) {
	if (_arrParams.Num() > 1 || _arrParams.Num() < 1) {
		return;
	}

	for (TActorIterator<ASpawnerActor> ActorItr(_pWorld); ActorItr; ++ActorItr) {
		FString name = ActorItr->GetName();
		if (name.Equals(_arrParams[0])) {
			ActorItr->SpawnActor();
		}
	}
}

void FTopDownEditorModule::CommandToolbar() {
	UE_LOG(LogTemp, Warning, TEXT("It Works!!!"));

	TSharedRef<SWindow> window = SNew(SWindow)
		.Title(FText::FromString("CONVERSATION WINDOW"))
		.SizingRule(ESizingRule::UserSized)
		.ClientSize(FVector2D(500,500));

	window.Get().SetContent(SNew(SHorizontalBox)
		+ SHorizontalBox::Slot().Padding(2, 5)
		[

			SNew(SVerticalBox)
			/*+ SVerticalBox::Slot().VAlign(VAlign_Top)
		[
			SNew(STextBlock).Text(FText::FromString("Conversation")).AutoWrapText(true)
			.Justification(ETextJustify::Center)
		]*/
		+SVerticalBox::Slot().VAlign(VAlign_Top)
		[
			SNew(STextBlock).Text(FText::FromString("CONVERSATION THINGS")).AutoWrapText(true)
			.WrappingPolicy(ETextWrappingPolicy::AllowPerCharacterWrapping)
		.Justification(ETextJustify::Center)
		]
		]
		+ SHorizontalBox::Slot().Padding(2, 5)
		[
			SNew(SListView<UObject*>)
		]);

	//TSharedRef<SWindow> parent = FSlateApplication::Get().GetActiveModalWindow();
	FSlateApplication::Get().AddWindow(window);

}
