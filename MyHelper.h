#pragma once
#include <fstream>
#include <iterator>
#include <string>
#include <Windows.h>
#include <vector>
#include <direct.h>
#include <ostream>
#include <sstream>
// YYTK 
#define YYSDK_PLUGIN // Declare the following code as a plugin
#include "SDK/SDK.hpp"
#include "Filesystem.h"

HINSTANCE DllHandle; // Self modhandle

std::string gPluginName = "Dumper";
YYTKPlugin* gThisPlugin = nullptr;
CallbackAttributes_t* callbackAttr = nullptr;

std::vector<std::string> obj_create_events; // holds all strings of code events that got triggered
int gDumpNum = 0;



namespace Misc {
    void Print(std::string s, Color c = CLR_DEFAULT)
    {
        PrintMessage(c, (gPluginName + ": " + s).c_str());
    }

    void Print(int i, Color c = CLR_DEFAULT)
    {
        PrintMessage(c, (gPluginName + ": " + std::to_string(i)).c_str());
    }

    void Print(const char* s, Color c = CLR_DEFAULT)
    {
        PrintMessage(c, (gPluginName + ": " + std::string(s)).c_str());
    }


    bool VectorContains(std::string find, std::vector<std::string>* v)
    {
        if (std::find(v->begin(), v->end(), find) != v->end())
        {
            return true;
        }
        return false;
    }

    bool AddToVectorNoDuplicates(std::string str, std::vector<std::string>* v)
    {
        if (VectorContains(str, v))
        {
            return false;
        }
        v->push_back(str);
        return true;
    }


    void VectorToFile(std::vector<std::string>* v)
    {
        std::string fname = "./event_dump_" + std::to_string(gDumpNum) + ".txt";
        while (Filesys::FileExists(fname))
        {
            fname = "./event_dump_" + std::to_string(gDumpNum) + ".txt";
            gDumpNum++;
        }

        Print("Dumping to file " + fname, CLR_RED);
        std::ofstream ofile(fname);
        std::ostream_iterator<std::string> oiter(ofile, "\n");
        std::copy(std::begin(*v), std::end(*v), oiter);
    }

    // checks if a string s contains a substring subs
    bool StringHasSubstr(std::string s, std::string subs)
    {
        if (s.find(subs) != std::string::npos) {
            return true;
        }
        return false;
    }

    // checks if the vector contains a string containing a substringand returns the whole string if found
    std::string VectorFindSubstring(std::vector<std::string> strings, std::string subs)
    {
        if (strings.size() == 0)
        {
            return "";
        }

        for (std::string s : strings)
        {
            if (StringHasSubstr(s, subs))
            {
                return s;
            }
        }
        return "";
    }

    //https://www.techiedelight.com/check-if-a-string-ends-with-another-string-in-cpp/
    bool StringEndsWith(std::string const& str, std::string const& suffix) {
        if (str.length() < suffix.length()) {
            return false;
        }
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }


    std::string Join(std::vector<std::string> strings)
    {
        if (strings.size() == 0)
        {
            return "";
        }

        const char* const delim = " ";
        std::ostringstream imploded;
        std::string ret;
        std::copy(strings.begin(), strings.end(), std::ostream_iterator<std::string>(imploded, delim));
        ret = imploded.str();
        ret.pop_back();

        return ret;
    }

    void PrintArray(YYRValue var, Color c = Color::CLR_DEFAULT)
    {
        YYRValue len;
        YYRValue item;
        CallBuiltin(len, "array_length_1d", nullptr, nullptr, { var });
        Misc::Print((int)len);

        for (int i = 0; i < (int)len ; i++)
        {
            CallBuiltin(item, "array_get", nullptr, nullptr, { var, (double)i });
            Misc::Print(static_cast<const char*>(item), c);
        }
    }

    void PrintArrayInstanceVariables(YYRValue var,YYRValue inst, Color c = Color::CLR_DEFAULT)
    {
        YYRValue len;
        YYRValue item;
        YYRValue content;
        YYRValue type;
        CallBuiltin(len, "array_length_1d", nullptr, nullptr, { var });
        Misc::Print((int)len);

        for (int i = 0; i < (int)len; i++)
        {
            CallBuiltin(item, "array_get", nullptr, nullptr, { var, (double)i });
            CallBuiltin(content, "variable_instance_get", nullptr, nullptr, { inst, static_cast<const char*>(item) });
            CallBuiltin(type, "typeof", nullptr, nullptr, { content });
            std::string typestr = std::string(static_cast<const char*>(type));
           /*/ Misc::Print(std::string(static_cast<const char*>(item)), c); //var name
            
            Misc::Print("->" +typestr);

            if (typestr == "number")
            {
                Misc::Print("->"+std::to_string((int)content));
            }
            else
            if (typestr == "bool")
            {
                Misc::Print("->" + std::to_string(int((bool)content)));
            }
            else if (typestr == "string")
            {
                Misc::Print("->" + std::string(static_cast<const char*>(content)));
            }
            */

            std::string message = std::string(static_cast<const char*>(item)) + " -> " + std::string(static_cast<const char*>(type));

            if (typestr == "number")
            {
                message += " : " + std::to_string((int)content);
            }
            else if (typestr == "bool")
            {
                message += " : " + std::to_string(int((bool)content));
            }
            else if (typestr == "string")
            {
                message += " : " + std::string(static_cast<const char*>(content));
            }

            Misc::Print(message);            
        }
    }


    void GetObjectInstanceVariables(YYRValue& arr, int objectType)
    {
        YYRValue nearest;
        CallBuiltin(nearest, "instance_nearest", nullptr, nullptr, { 0.0,0.0,(double)objectType });
        Misc::Print((int)nearest);

        CallBuiltin(arr, "variable_instance_get_names", nullptr, nullptr, { nearest });

    }

    void GetInstanceVariables(YYRValue& arr, YYRValue inst)
    {
        CallBuiltin(arr, "variable_instance_get_names", nullptr, nullptr, { inst });
    }

    void GetFirstOfObject(YYRValue& inst, int objType)
    {    
        CallBuiltin(inst, "instance_nearest", nullptr, nullptr, { 0.0,0.0,(double)objType });
        Misc::Print((int)inst);
    }

    const RefDynamicArrayOfRValue* ResolveArray(YYRValue var)
    {
        RValue gvrval = var.As<RValue>();
        const RefDynamicArrayOfRValue* thisarr = reinterpret_cast<const RefDynamicArrayOfRValue*>(gvrval.RefArray);
        return thisarr;
    }

    YYRValue CallBuiltin(const std::string name, CInstance* self, CInstance* other,const std::vector<YYRValue> args)
    {
        YYRValue var;
        CallBuiltin(var, name, self, other, args);
        return var;
    }
}