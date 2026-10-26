#include "Week04AssetLibrary.h"
#include "Week04/TargetCrate.h"
#include "Engine/Blueprint.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_Event.h"
#include "K2Node_CallFunction.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
bool UWeek04AssetLibrary::ConfigureCrateBlueprint(UBlueprint* Blueprint,bool bSolution)
{
    if(!Blueprint || !Blueprint->ParentClass->IsChildOf(ATargetCrate::StaticClass()))return false;
    auto* Graph=FBlueprintEditorUtils::FindEventGraph(Blueprint);if(!Graph)return false;
    const auto* Schema=GetDefault<UEdGraphSchema_K2>();
    auto Place=[&](UEdGraphNode* Node,int32 X){Graph->AddNode(Node,false,false);Node->CreateNewGuid();Node->NodePosX=X;Node->NodePosY=0;Node->AllocateDefaultPins();};
    auto* Event=NewObject<UK2Node_Event>(Graph);
    Event->EventReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(ATargetCrate,ReceiveDeathVisual),ATargetCrate::StaticClass());Event->bOverrideFunction=true;Place(Event,0);
    bool OK=true;
    if(bSolution)
    {
        auto* Hide=NewObject<UK2Node_CallFunction>(Graph);Hide->SetFromFunction(AActor::StaticClass()->FindFunctionByName(TEXT("SetActorHiddenInGame")));Place(Hide,330);
        auto* Disable=NewObject<UK2Node_CallFunction>(Graph);Disable->SetFromFunction(AActor::StaticClass()->FindFunctionByName(TEXT("SetActorEnableCollision")));Place(Disable,670);
        Schema->TrySetDefaultValue(*Hide->FindPin(TEXT("bNewHidden")),TEXT("true"));
        Schema->TrySetDefaultValue(*Disable->FindPin(TEXT("bNewActorEnableCollision")),TEXT("false"));
        OK=Schema->TryCreateConnection(Event->FindPin(UEdGraphSchema_K2::PN_Then),Hide->FindPin(UEdGraphSchema_K2::PN_Execute));
        OK &= Schema->TryCreateConnection(Hide->FindPin(UEdGraphSchema_K2::PN_Then),Disable->FindPin(UEdGraphSchema_K2::PN_Execute));
    }
    else {Event->NodeComment=TEXT("LAB: add a visible death response and disable collision after the death event");Event->bCommentBubbleVisible=true;}
    FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(Blueprint);FKismetEditorUtilities::CompileBlueprint(Blueprint);
    return OK && Blueprint->Status!=BS_Error;
}
