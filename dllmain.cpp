// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>

// YYTK is in this now
#include "MyHelper.h"
#include "Assets.h"
#include "LHSprites.h"
#include "LHObjects.h"
// Plugin functionality
#include <fstream>
#include <iterator>
#include <map>
#include <iostream>


#define _CRT_SECURE_NO_WARNINGS

bool active = false;

void addExternalSprite()
{
    //s_hero_attack_rytsar
    //double idx = Assets::AddSprite("arba_spritesheet.png", 11, true, true, 64,54);
    //Misc::Print(std::to_string(idx));

    //Misc::Print(std::to_string(Assets::GetSpriteImgnum(LHSpriteEnum::s_hero_attack_rytsar)));
    //Misc::Print(std::to_string(Assets::GetSpriteOffsetX(LHSpriteEnum::s_hero_attack_rytsar)));
    //Misc::Print(std::to_string(Assets::GetSpriteOffsetY(LHSpriteEnum::s_hero_attack_rytsar)));

    // Replace s_hero_attack_rytsar
    Assets::SpriteReplace(LHSpriteEnum::s_hero_attack_rytsar, "Assets\\arba_attack.png", 4, true, false, 0.0, 0.0);
    Assets::SpriteReplace(LHSpriteEnum::s_hero_idle_rytsar, "Assets\\arba_idle.png", 1, true, false, 0.0, 0.0);
    Assets::SpriteReplace(LHSpriteEnum::s_hero_charge_rytsar, "Assets\\arba_idle.png", 1, true, false, 0.0, 0.0);
    Assets::SpriteReplace(LHSpriteEnum::s_hero_hurt_rytsar, "Assets\\arba_hurt.png", 1, true, false, 0.0, 0.0);
    Assets::SpriteReplace(LHSpriteEnum::s_hero_warrior, "Assets\\arba_map.png", 4, true, false, 0.0, 0.0);
}

void mapToFile(std::map<int, std::string> arg, std::string fname, std::string enumName)
{
    std::ofstream outFile(fname);

    if (outFile.is_open()) {
        outFile << "#pragma once\n\n";
        outFile << "enum "<< enumName <<" {\n";

        for (const auto& entry : arg) {
            outFile << "    " << (entry.second) << " = " <<(entry.first) << ",\n";
        }

        outFile << "};\n";

        outFile.close();
        std::cout << fname <<" file generated successfully.\n";
    }
    else {
        std::cerr << "Unable to open file for writing.\n";
    }
}

void dumpObjectIDs()
{
    std::map<int, std::string> objMap;

    while (true)
    {
        YYRValue doesExist = Misc::CallBuiltin("object_exists", nullptr, nullptr, { static_cast<double>(objMap.size()) });
        Misc::Print(" Does exist: " + std::to_string(doesExist.As<double>()));
        if (static_cast<double>(doesExist) == 0.0)
        {
            Misc::Print("Done!");
            break;
        }
        YYRValue name = Misc::CallBuiltin("object_get_name", nullptr, nullptr, { static_cast<double>(objMap.size()) });
        // cast to string
        std::string namestr = static_cast<const char*>(name);
        Misc::Print(namestr);
        // add to map
        objMap.insert(std::pair<int, std::string>(objMap.size(), namestr));
    }

    mapToFile(objMap, "objEnum.txt", "LHObjectEnum");
}

void dumpSpriteIDs()
{
    // sprite ID ; sprite name
    std::map<int, std::string> spriteMap;
    // Loop through all sprites
    while(true)
    {
        YYRValue doesExist = Misc::CallBuiltin("sprite_exists", nullptr, nullptr, { static_cast<double>(spriteMap.size()) });
        Misc::Print(" Does exist: " + std::to_string(doesExist.As<double>()));
        if (static_cast<double>(doesExist) == 0.0)
        {
            Misc::Print("Done!");
            break;
        }
        YYRValue spriteName = Misc::CallBuiltin("sprite_get_name", nullptr, nullptr, { static_cast<double>(spriteMap.size()) });
        // cast to string
        std::string spriteNameStr = static_cast<const char*>(spriteName);
        Misc::Print(spriteNameStr);
        // add to map
        spriteMap.insert(std::pair<int, std::string>(spriteMap.size(), spriteNameStr));
    }

    Misc::Print("Map has " + std::to_string(spriteMap.size()));

    std::ofstream outFile("SpriteEnum.txt");

    if (outFile.is_open()) {
        outFile << "#pragma once\n\n";
        outFile << "enum SpriteEnum {\n";

        for (const auto& entry : spriteMap) {
            outFile << "    " << entry.second << " = " << entry.first << ",\n";
        }

        outFile << "};\n";

        outFile.close();
        std::cout << "SpriteEnum.txt file generated successfully.\n";
    }
    else {
        std::cerr << "Unable to open file for writing.\n";
    }

}

void enableDebug()
{
    Misc::CallBuiltin("show_debug_overlay", nullptr, nullptr, {1.0});
}

void arrayShit()
{
    YYRValue arr;
    CallBuiltin(arr, "array_create", nullptr, nullptr, { 5.0, 69.0 });

    Misc::Print((int)arr);
    YYRValue len;
    CallBuiltin(len, "array_length_1d", nullptr, nullptr, {arr});

    Misc::Print((int)len);

    YYRValue item;
    CallBuiltin(item, "array_get", nullptr, nullptr, { arr, 0.0 });

    Misc::Print((int)item);


    // get global arr
    YYRValue globalVars;
    std::vector<YYRValue> globalVarArgs;
    globalVarArgs.push_back(-5.0);
    CallBuiltin(globalVars, "variable_instance_get_names", nullptr, nullptr, globalVarArgs);

    CallBuiltin(len, "array_length_1d", nullptr, nullptr, { globalVars });
    Misc::Print((int)len);
    
    for (int i = 0; i < (int)len - 1; i++)
    {
        CallBuiltin(item, "array_get", nullptr, nullptr, { globalVars, (double)i });
        Misc::Print(static_cast<const char*>(item));
    }

    // Get all vars of buttons
    YYRValue nearest;
    CallBuiltin(nearest, "instance_nearest", nullptr, nullptr, { 0.0,0.0,(double)LHObjectEnum::o_opt_lang_button });
    Misc::Print((int)nearest);

    CallBuiltin(globalVars, "variable_instance_get_names", nullptr, nullptr, {nearest});

    Misc::PrintArray(globalVars, Color::CLR_AQUA);
    
}

void dumpVars()
{
    Misc::Print("Dumping shit:");
    YYRValue vars;
    //Misc::GetObjectInstanceVariables(vars, LHObjectEnum::o_camp_build_button);
    //Misc::PrintArray(vars, Color::CLR_BRIGHTPURPLE);

    YYRValue obj;
    Misc::GetFirstOfObject(obj, LHObjectEnum::o_camp_build_button);
    // show text
    YYRValue text;
    CallBuiltin(text, "variable_instance_get", nullptr, nullptr, { obj, "text" });

    Misc::Print(static_cast<const char*>(text), Color::CLR_GOLD);

    CallBuiltin(text, "variable_instance_set", nullptr, nullptr, { obj, "text", "ayo lmao" });
}

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


    if ( Misc::StringHasSubstr(codeObj->i_pName, "o_card") && Misc::StringHasSubstr(codeObj->i_pName, "gml_Object") && (Misc::StringHasSubstr(codeObj->i_pName, "create") || Misc::StringHasSubstr(codeObj->i_pName, "Create")))
    {

        // Dump
        
        /*
        YYRValue nearest;
        CallBuiltin(nearest, "instance_nearest", nullptr, nullptr, { 0.0,0.0,(double)LHObjectEnum::o_hero });
        
        YYRValue spd;
        CallBuiltin(spd, "variable_instance_get", selfInst, otherInst, {nearest, "spd"});

        Misc::Print((int)spd, Color::CLR_GOLD);
        */
    }

    if (Misc::StringHasSubstr(codeObj->i_pName, "gml_Object") && (Misc::StringHasSubstr(codeObj->i_pName, "create") || Misc::StringHasSubstr(codeObj->i_pName, "Create")))
    {
        
        PrintMessage(CLR_DEFAULT, "%s SpriteID: %d", codeObj->i_pName, selfInst->i_spriteindex);
 
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
        if (GetAsyncKeyState(VK_NUMPAD2))
        {
            Misc::Print("Dumping sprite names with ID");
            dumpSpriteIDs();
            Sleep(300);
        }
        if (GetAsyncKeyState(VK_NUMPAD3))
        {
            Misc::Print("Dumping object names with ID");
            dumpObjectIDs();
            Sleep(300);
        }
        if (GetAsyncKeyState(VK_NUMPAD4))
        {
            Misc::Print("Turning on Debug mode");
            enableDebug();
            Sleep(300);
        }
        if (GetAsyncKeyState(VK_NUMPAD5))
        {
            Misc::Print("bruh");
            arrayShit();
            Sleep(300);
        }
        if (GetAsyncKeyState(VK_NUMPAD6))
        {
            Misc::Print("dumping var names");
            dumpVars();
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

