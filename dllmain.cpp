// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>

// YYTK is in this now
#include "MyHelper.h"

// Plugin functionality
#include <fstream>
#include <iterator>
#define _CRT_SECURE_NO_WARNINGS

// Unload
YYTKStatus PluginUnload()
{
    PmRemoveCallback(callbackAttr);

    return YYTK_OK;
}

YYTKStatus ExecuteCodeCallback(YYTKCodeEvent* codeEvent, void*)
{
    CCode* codeObj = std::get<CCode*>(codeEvent->Arguments());
    CInstance* selfInst = std::get<0>(codeEvent->Arguments());
    CInstance* otherInst = std::get<1>(codeEvent->Arguments());

    // If we have invalid data???
    if (!codeObj)
        return YYTK_INVALIDARG;

    if (!codeObj->i_pName)
        return YYTK_INVALIDARG;

    // Original event
    //codeEvent->Call(selfInst, otherInst, codeObj, std::get<3>(codeEvent->Arguments()), std::get<4>(codeEvent->Arguments()));

    if (Misc::StringHasSubstr(codeObj->i_pName, "gml_Room_"))
    {
        Misc::Print("Room Change: " + std::string(codeObj->i_pName));
        
        YYRValue gamespeed;
        CallBuiltin(gamespeed, "game_get_speed", selfInst, otherInst, {0.0});

        Misc::Print("Gamespeed: " + std::to_string(static_cast<double>(gamespeed)));

        YYRValue gamespeedmillis;
        CallBuiltin(gamespeedmillis, "game_get_speed", selfInst, otherInst, { 1.0 });

        Misc::Print("Gamespeed: " + std::to_string(static_cast<double>(gamespeedmillis)));
        
    }


    
   
    return YYTK_OK;
}


// Entry
DllExport YYTKStatus PluginEntry(
    YYTKPlugin* PluginObject // A pointer to the dedicated plugin object
)
{
    Misc::Print("Hello World");
    gThisPlugin = PluginObject;
    gThisPlugin->PluginUnload = PluginUnload;

    PluginAttributes_t* pluginAttributes = nullptr;
    if (PmGetPluginAttributes(gThisPlugin, pluginAttributes) == YYTK_OK)
    {
        PmCreateCallback(pluginAttributes, callbackAttr, reinterpret_cast<FNEventHandler>(ExecuteCodeCallback), EVT_CODE_EXECUTE, nullptr);
    }


    // Initialize the plugin, set callbacks inside the PluginObject.
    // Set-up buffers.
    return YYTK_OK; // Successful PluginEntry.
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

