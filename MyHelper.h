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

HINSTANCE DllHandle; // Self modhandle

std::string gPluginName = "Dumper";
YYTKPlugin* gThisPlugin = nullptr;
CallbackAttributes_t* callbackAttr = nullptr;




namespace Misc {
    void Print(std::string s, Color c = CLR_DEFAULT)
    {
        PrintMessage(c, (gPluginName + ": " + s).c_str());
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


    inline bool FileExists(const std::string& name) {
        if (FILE* file = fopen(name.c_str(), "r")) {
            fclose(file);
            return true;
        }
        else {
            return false;
        }
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

    std::string GetCurrentDir() // Returns EXE directory
    {
        char cCurrentPath[FILENAME_MAX]; // get working directory into buffer
        if (!_getcwd(cCurrentPath, sizeof(cCurrentPath)))
            exit(-1);
        cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; // not really required

        char* s = cCurrentPath; // save path from buffer into currentpath chararr
        std::string str(s);
        //free(s);
        return str;
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

    void PrintArray(YYRValue var)
    {
        RValue gvrval = var.As<RValue>();

        const RefDynamicArrayOfRValue* array = reinterpret_cast<const RefDynamicArrayOfRValue*>(gvrval.RefArray);


        for (int i = 0; i < array->length; i++) {
            const RValue& element = array->m_Array[i];
            if (element.Kind == VALUE_STRING)
            {
                Misc::Print(element.String->Get());
            }
        }
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
        CallBuiltin(var, "variable_instance_get_names", self, other, args);
        return var;
    }
}