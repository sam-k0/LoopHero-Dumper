// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>

// YYTK is in this now
#include "MyHelper.h"

// Plugin functionality
#include <fstream>
#include <iterator>
#define _CRT_SECURE_NO_WARNINGS

bool active = false;

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


    if (Misc::StringHasSubstr(codeObj->i_pName, "gml_Room_"))
    {
        Misc::Print("Room Change: " + std::string(codeObj->i_pName));
        
        YYRValue gamespeed;
        CallBuiltin(gamespeed, "game_get_speed", selfInst, otherInst, {0.0});

        Misc::Print("Gamespeed: " + std::to_string(static_cast<double>(gamespeed)));

        YYRValue gamespeedmillis;
        CallBuiltin(gamespeedmillis, "game_get_speed", selfInst, otherInst, { 1.0 });

        Misc::Print("Gamespeed: " + std::to_string(static_cast<double>(gamespeedmillis)));
        
        Misc::CallBuiltin("game_set_speed", selfInst, otherInst, { 1.0, 1.0 });

        active = true;
    }
    else if( Misc::StringHasSubstr(codeObj->i_pName, "gml_Object") && ( Misc::StringHasSubstr(codeObj->i_pName, "create") || Misc::StringHasSubstr(codeObj->i_pName, "Create")))
    {
        if(active)
        {
            Misc::Print(codeObj->i_pName + std::string(" ") + std::to_string(selfInst->i_id) );
            Misc::AddToVectorNoDuplicates(codeObj->i_pName, &obj_create_events);
        }
        
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

DWORD WINAPI Menu(HINSTANCE hModule)
{
    while (true)
    {
        Sleep(50);
        if (GetAsyncKeyState(VK_NUMPAD0))
        {
            Misc::Print("Dumping to file!");
            Misc::VectorToFile(&obj_create_events);
            Sleep(300);
        }
        if (GetAsyncKeyState(VK_NUMPAD1))
        {
            Misc::Print("Resetting event vector");
            obj_create_events.clear();
            Sleep(300);
        }
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DllHandle = hModule;
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Menu, NULL, 0, NULL); // For the input
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

