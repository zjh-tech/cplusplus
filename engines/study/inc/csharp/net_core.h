/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-05-06 11:21:02
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-05-06 18:15:25
 */

#pragma once

#include "engine/inc/common/utils.hpp"
#include "engine/inc/csharp/coreclrhost.h"
#include <experimental/filesystem>
#include <iostream>
#include <limits.h>
#include <set>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#if defined(__linux__)
#  include <dirent.h>
#  include <dlfcn.h>
#else
#  include "windows.h"
#endif

using namespace std;
using namespace std::experimental::filesystem::v1;

using namespace Framework;

namespace Framework {
namespace CSharp {

  class CNetCore {
  public:
    CNetCore();
    virtual ~CNetCore();

  public:
    bool Init(const char* app_path, const char* pkg_path);

    //need be free : string linux free  window CoTaskMemFree,other ...
    bool GetCSharpProcAddress(const char* name_space, const char* class_name, const char* func_name, void** out_delegate);

  private:
    void AddFilesFromDirectoryToTpaList(const char* directory, std::string& tpaList);

  private:
#if defined(__linux__)
    void* m_coreclr = nullptr;
#else
    HMODULE m_coreclr = nullptr;
#endif
    unsigned int m_domain_id = 0;
    void*        m_handle    = nullptr;

    coreclr_create_delegate_ptr m_coreclr_create_delegate = nullptr;
  };

}  // namespace CSharp
}  // namespace Framework