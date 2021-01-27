/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2019-12-13 10:57:25
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-07 11:12:59
 */

#pragma once

#include "engine/inc/common/singleton.hpp"
#include <map>
#include <string>

using namespace std;

namespace Framework {

typedef void* (*CreateObjPtr)();

class ClassFactory : public Singleton<ClassFactory> {
public:
  void RegisterClass(const string& classname, CreateObjPtr method) {
    auto iter = m_class_map.find(classname);
    if (iter != m_class_map.end()) {
      return;
    }

    m_class_map.insert(pair<std::string, CreateObjPtr>(classname, method));
  }

  void* GetClassByName(const std::string& classname) {
    auto iter = m_class_map.find(classname);
    if (iter == m_class_map.end()) {
      return nullptr;
    }

    return iter->second();
  }

private:
  map<std::string, CreateObjPtr> m_class_map;
};

class RegisterClassAction {
public:
  RegisterClassAction(const string& class_name, CreateObjPtr create_ptr) {
    ClassFactory::Instance()->RegisterClass(class_name, create_ptr);
  }
};

#define REGISTER_REFLECTOR(classname)    \
  classname* ObejctCreate##classname() { \
    return new classname();              \
  }                                      \
  Framework::RegisterClassAction g_register##classname(#classname, (Framework::CreateObjPtr)ObejctCreate##classname);

}  // namespace Framework