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


std::vector<YYRValue> SwapCards;
int swapCooldown = 60;
CallbackAttributes_t* frameCallbackAttr;

int lastSurf = -1;

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

void arrayShit(int iid, std::string arrname)
{
    YYRValue type;
    YYRValue arr = Misc::CallBuiltin("variable_instance_get", nullptr, nullptr, { double(iid), arrname });
    CallBuiltin(type, "typeof", nullptr, nullptr, { arr });
    std::string typestr = std::string(static_cast<const char*>(type));
    if (typestr == "array")
    {
        Misc::PrintArray(arr);
    }
   
    
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

void printAllObjects()
{
    YYRValue allObjs;
    YYRValue iid;
    YYRValue arr;
    CallBuiltin(allObjs, "instance_number", nullptr, nullptr, { INSTANCE_ALL });
    // ic = instance_count(all)
    // instance_id_get(ic-1)
    for (int i = 0; i < ((int)allObjs) - 1; i++)
    {
        CallBuiltin(iid, "instance_id_get", nullptr, nullptr, { (double)i });
        Misc::GetInstanceVariables(arr, iid);
        Misc::Print("______________");
        Misc::PrintArray(arr);
    }
    
}


void getObjectAtMousePos()
{
    YYRValue mx = Misc::CallBuiltin("device_mouse_x", nullptr, nullptr, { 0.0 });
    YYRValue my = Misc::CallBuiltin("device_mouse_y", nullptr, nullptr, { 0.0 });
    
    YYRValue obj = Misc::CallBuiltin("instance_nearest", nullptr, nullptr, {mx, my, INSTANCE_ALL });

    YYRValue arr;
    Misc::GetInstanceVariables(arr, obj);
    Misc::Print("______________" + std::to_string((int)obj), Color::CLR_AQUA);
    Misc::PrintArrayInstanceVariables(arr,obj, Color::CLR_AQUA);
}

// Unload
YYTKStatus PluginUnload()
{
    PmRemoveCallback(callbackAttr);

    return YYTK_OK;
}

YYTKStatus FrameCallback(YYTKEventBase* pEvent, void* optArgument)
{
    if (swapCooldown > 0)
    {
        swapCooldown -= 1;
    }    
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
    
    if (Misc::StringHasSubstr(codeObj->i_pName, "gml_Room_rm_game_Create"))
    {
        SwapCards.clear();
    }

    if (Misc::StringHasSubstr(codeObj->i_pName, "statistik_Draw_0"))
    {
        //PrintMessage(CLR_DEFAULT, "%s SpriteID: %d", codeObj->i_pName, selfInst->i_spriteindex);
        // Check if surf exists, free it, create new, assign
        YYRValue statsPanel = Misc::CallBuiltinA("instance_nearest", {32.0,32.0,double(LHObjectEnum::o_camp_statistik)});
        YYRValue origsurf = Misc::CallBuiltinA("variable_instance_get", {statsPanel, "statsurf"});
        Misc::CallBuiltinA("variable_instance_set", { statsPanel, "text","Playing modded" });
        if ((int)origsurf != lastSurf)
        {
            YYRValue exists = Misc::CallBuiltinA("surface_exists", { origsurf });
            if ((bool)exists == true) //Draw
            {
                //Misc::Print("surf exists");
                if ((bool)Misc::CallBuiltinA("surface_set_target", { origsurf }) == true)
                {
                    // save orig draw vars
                    YYRValue halign = Misc::CallBuiltinA("draw_get_halign", {  });
                    YYRValue font = Misc::CallBuiltinA("draw_get_font", {});
                    // get surf vars
                    YYRValue surfw = Misc::CallBuiltinA("surface_get_width", { origsurf });
                    YYRValue surfh = Misc::CallBuiltinA("surface_get_height", { origsurf });

                    // draw
                    Misc::CallBuiltinA("draw_set_font", { 1.0 });

                    YYRValue col = Misc::CallBuiltinA("make_color_rgb", {96.,34.,23.});
                    Misc::CallBuiltinA("draw_set_color", { col });
                    Misc::CallBuiltinA("draw_rectangle", { 0.0,0.0,surfw, surfh, 0.0 });
                    
                    col = Misc::CallBuiltinA("make_color_rgb", { 255.,255.,255. });
                    Misc::CallBuiltinA("draw_set_color", { 16777215.0 });
                    Misc::CallBuiltinA("draw_set_halign", { 0.0 });
                    Misc::CallBuiltinA("draw_text_transformed", {32.,32.,"Hello from mods!",1.0,1.0,0.0 });
                    // reset
                    Misc::CallBuiltinA("surface_reset_target", { });
                    Misc::CallBuiltinA("draw_set_halign", {halign});
                    Misc::CallBuiltinA("draw_set_font", { font });
                }
            }
        }
        lastSurf = (int)origsurf;
        
    }

    if (Misc::StringHasSubstr(codeObj->i_pName, "o_menu_Draw_0"))
    {
        for (int i = 0; i < SwapCards.size(); i++)
        {
            double dx = Misc::CallBuiltin("variable_instance_get", nullptr, nullptr, { SwapCards[i], "xx"});
            double dy = Misc::CallBuiltin("variable_instance_get", nullptr, nullptr, { SwapCards[i], "yy" });

            YYRValue sprite = Misc::CallBuiltin("variable_instance_get", nullptr, nullptr, { SwapCards[i], "sprite_index" });
            double sw, sh;
            Assets::GetSpriteDimensions((double)sprite, sw, sh);
            Misc::CallBuiltin("draw_rectangle", nullptr, nullptr, {dx, dy, dx+sw,dy-sh, 1.0});

        }
    }

    // Left mouse
    if ( Misc::StringHasSubstr(codeObj->i_pName, "o_card") && Misc::StringHasSubstr(codeObj->i_pName, "gml_Object") && Misc::StringHasSubstr(codeObj->i_pName, "Mouse_56"))
    {       
        YYRValue mx = Misc::CallBuiltin("device_mouse_x", nullptr, nullptr, { 0.0 });
        YYRValue my = Misc::CallBuiltin("device_mouse_y", nullptr, nullptr, { 0.0 });            
        YYRValue cardid = Misc::CallBuiltin("instance_place", selfInst, otherInst, { double(mx)-16, my, (double)LHObjectEnum::o_card });
           

        if ((int)cardid == INSTANCE_NOONE || (swapCooldown > 0))
        {
            return YYTK_OK;            
        }

        Misc::Print("Picked up: " + std::to_string(int(cardid)));
        Misc::Print("Len is : " + std::to_string(SwapCards.size()));
        // Clear vector
        SwapCards.clear();
        SwapCards.push_back(cardid);
    }
    // Right mouse
    if (Misc::StringHasSubstr(codeObj->i_pName, "o_card") && Misc::StringHasSubstr(codeObj->i_pName, "gml_Object") && Misc::StringHasSubstr(codeObj->i_pName, "Mouse_54"))
    {
        YYRValue mx = Misc::CallBuiltin("device_mouse_x", nullptr, nullptr, { 0.0 });
        YYRValue my = Misc::CallBuiltin("device_mouse_y", nullptr, nullptr, { 0.0 });
        YYRValue cardid = Misc::CallBuiltin("instance_place", selfInst, otherInst, { double(mx)-16, my, (double)LHObjectEnum::o_card });
        if ((int)cardid == INSTANCE_NOONE || (swapCooldown > 0) || SwapCards.empty())
        {
            return YYTK_OK;
        }

        Misc::Print("Picked up: " + std::to_string(int(cardid)));
        SwapCards.push_back(cardid);
        Misc::Print("Len is : " + std::to_string(SwapCards.size()));
        //swap
        Misc::Print("swap: " + std::to_string(int(SwapCards[0])) + "/" + std::to_string(int(SwapCards[1])));

        // check if they still exist
        if (double(Misc::CallBuiltin("instance_exists", nullptr, nullptr, { SwapCards[0] })) == 1.0
            && double(Misc::CallBuiltin("instance_exists", nullptr, nullptr, { SwapCards[1] })) == 1.0)
        {
            // get vars
            YYRValue firstnum = Misc::CallBuiltin("variable_instance_get", nullptr, nullptr, { SwapCards[0] , "card_number" });
            YYRValue secondnum = Misc::CallBuiltin("variable_instance_get", nullptr, nullptr, { SwapCards[1] , "card_number" });
            Misc::Print("nums: " + std::to_string(int(firstnum)) + "/" + std::to_string(int(secondnum)));
            Misc::CallBuiltin("variable_instance_set", nullptr, nullptr, { SwapCards[0] , "card_number", secondnum });
            Misc::CallBuiltin("variable_instance_set", nullptr, nullptr, { SwapCards[1] , "card_number", firstnum});
        }

        SwapCards.clear();
    }
    /*if (Misc::StringHasSubstr(codeObj->i_pName, "gml_Object") && (Misc::StringHasSubstr(codeObj->i_pName, "create") || Misc::StringHasSubstr(codeObj->i_pName, "Create")))
    {
        
        PrintMessage(CLR_DEFAULT, "%s SpriteID: %d", codeObj->i_pName, selfInst->i_spriteindex);
 
    }*/
    


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
        PmCreateCallback(pluginAttributes, frameCallbackAttr, FrameCallback, static_cast<EventType>(EVT_PRESENT | EVT_ENDSCENE), nullptr);
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
            YYRValue iid = Misc::CallBuiltin("get_integer", nullptr, nullptr, { "instance id", 0.0 });
            YYRValue str = Misc::CallBuiltin("get_string", nullptr, nullptr, { "var name", "text" });
            arrayShit(iid, str);
            Sleep(300);
        }
        if (GetAsyncKeyState(VK_NUMPAD6))
        {
            Misc::Print("dumping var names");
            printAllObjects();
            Sleep(300);
        }
        if (GetAsyncKeyState(VK_NUMPAD7))
        {
            Misc::Print("obj at pos");
            getObjectAtMousePos();
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

