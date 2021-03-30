#pragma once

#include <map>
#include <string>

#include "engine/inc/common/singleton.hpp"

using namespace std;

namespace Framework
{
    typedef void* (*CreateObjPtr)();

    class ClassFactory : public Singleton<ClassFactory>
    {
    public:
        void RegisterClass(const string& classname, CreateObjPtr method)
        {
            auto iter = class_map_.find(classname);
            if (iter != class_map_.end())
            {
                return;
            }

            class_map_.insert(pair<std::string, CreateObjPtr>(classname, method));
        }

        void* GetClassByName(const std::string& classname)
        {
            auto iter = class_map_.find(classname);
            if (iter == class_map_.end())
            {
                return nullptr;
            }

            return iter->second();
        }

    private:
        map<std::string, CreateObjPtr> class_map_;
    };

    class RegisterClassAction
    {
    public:
        RegisterClassAction(const string& class_name, CreateObjPtr create_ptr)
        {
            ClassFactory::Instance()->RegisterClass(class_name, create_ptr);
        }
    };

#define REGISTER_REFLECTOR(classname)    \
    classname* ObejctCreate##classname() \
    {                                    \
        return new classname();          \
    }                                    \
    Framework::RegisterClassAction g_register##classname(#classname, (Framework::CreateObjPtr)ObejctCreate##classname);

}  // namespace Framework