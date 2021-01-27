/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-05-06 11:22:57
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-05-06 16:19:36
 */

#include "engine/inc/csharp/net_core.h"
#include "engine/inc/log/logger.h"

namespace Framework {
namespace CSharp {

  CNetCore::CNetCore() {
  }

  CNetCore::~CNetCore() {
    if (m_coreclr) {
      if (m_handle && m_domain_id != 0) {
#if defined(__linux__)
        coreclr_shutdown_ptr coreclr_shutdown = reinterpret_cast<coreclr_shutdown_ptr>(dlsym(m_coreclr, "coreclr_shutdown"));
#else
        coreclr_shutdown_ptr coreclr_shutdown = (coreclr_shutdown_ptr)GetProcAddress(m_coreclr, "coreclr_shutdown");
#endif
        if (coreclr_shutdown) {
          coreclr_shutdown(m_handle, m_domain_id);
        }
      }

      m_handle    = nullptr;
      m_domain_id = 0;
    }

    if (m_coreclr) {
#if defined(__linux__)
      dlclose(m_coreclr);
#else
      FreeLibrary(m_coreclr);
#endif
      m_coreclr = nullptr;
    }
  }

  bool CNetCore::Init(const char* app_path, const char* pkg_path) {
    assert(app_path && pkg_path);

    char real_app_path[MAX_PATH_SIZE] = {0};
    char real_pkg_path[MAX_PATH_SIZE] = {0};
    if (Utils::RealPath(app_path, real_app_path) == NULL) {
      LogErrorA("[netcore] {} realpath fail", app_path);
      return false;
    }

    if (Utils::RealPath(pkg_path, real_pkg_path) == NULL) {
      LogErrorA("[netcore] {} realpath fail", pkg_path);
      return false;
    }

    LogInfoA("[netcore] Loading CoreCLR...");

    std::string coreclr_path(real_pkg_path);
#if defined(__linux__)
    coreclr_path.append("/libcoreclr.so");
    m_coreclr = dlopen(coreclr_path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (m_coreclr == nullptr) {
      LogErrorA("[netcore]  dlopen {} fail {}", coreclr_path, dlerror());
      return false;
    }

    coreclr_initialize_ptr coreclr_init = reinterpret_cast<coreclr_initialize_ptr>(dlsym(m_coreclr, "coreclr_initialize"));
    m_coreclr_create_delegate           = reinterpret_cast<coreclr_create_delegate_ptr>(dlsym(m_coreclr, "coreclr_create_delegate"));
#else
    coreclr_path.append("/coreclr.dll");
    m_coreclr = LoadLibraryExA(coreclr_path.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (m_coreclr == nullptr) {
      LogErrorA("[netcore]  LoadLibraryEx {} fail errorcode = {}", coreclr_path, GetLastError());
      return false;
    }

    coreclr_initialize_ptr coreclr_init = reinterpret_cast<coreclr_initialize_ptr>(GetProcAddress(m_coreclr, "coreclr_initialize"));
    m_coreclr_create_delegate = reinterpret_cast<coreclr_create_delegate_ptr>(GetProcAddress(m_coreclr, "coreclr_create_delegate"));
#endif

    if (coreclr_init == nullptr) {
      LogErrorA("[netcore]  coreclr_initialize  fail");
      return false;
    }

    if (m_coreclr_create_delegate == nullptr) {
      LogErrorA("[netcore]  coreclr_create_delegate  fail");
      return false;
    }

    string tpa_list;
    AddFilesFromDirectoryToTpaList(real_pkg_path, tpa_list);

    const char* property_keys[] = {
      "APP_PATHS",
      "TRUSTED_PLATFORM_ASSEMBLIES"};

    const char* property_values[] = {
      // APP_PATHS
      real_app_path,
      // TRUSTED_PLATFORM_ASSEMBLIES
      tpa_list.c_str()};

    int error_code = coreclr_init(
      app_path,                                 // exePath
      "host",                                   // appDomainFriendlyName
      sizeof(property_values) / sizeof(char*),  // propertyCount
      property_keys,                            // propertyKeys
      property_values,                          // propertyValues
      &m_handle,                                // hostHandle
      &m_domain_id                              // domainId
    );

    if (error_code < 0) {
      LogErrorA("[netcore]  Initializing CoreCLR Fail");
      return false;
    }

    LogInfoA("[netcore] Initializing CoreCLR Success");
    return true;
  }

  void CNetCore::AddFilesFromDirectoryToTpaList(const char* directory, std::string& tpaList) {
    if (!exists(directory)) {
      return;
    }

    directory_iterator list(directory);
    const char* const  tpaExtensions[] = {
      ".dll",
      //".ni.dll",  // Probe for .ni.dll first so that it's preferred if ni and il coexist in the same dir
      // ".ni.exe",
      //".exe",
    };

    std::set<std::string> assemblies;

    size_t ext_total_index = sizeof(tpaExtensions) / sizeof(tpaExtensions[0]);
    for (size_t extIndex = 0; extIndex < ext_total_index; ++extIndex) {
      const char* ext       = tpaExtensions[extIndex];
      size_t      extLength = strlen(ext);

      for (auto& iter : list) {
        std::string filename = iter.path().filename().string();
        if (filename.length() < extLength) {
          continue;
        }

        size_t extPos = filename.length() - extLength;
        if ((extPos <= 0) || (filename.compare(extPos, extLength, ext) != 0)) {
          continue;
        }

        std::string filenameWithoutExt(filename.substr(0, extPos));
        if (assemblies.find(filenameWithoutExt) == assemblies.end()) {
          assemblies.insert(filenameWithoutExt);

          tpaList.append(directory);
          tpaList.append("/");
          tpaList.append(filename);
#if defined(__linux__)
          tpaList.append(":");
#else
          tpaList.append(";");
#endif
        }
      }
    }
  }

  bool CNetCore::GetCSharpProcAddress(const char* name_space, const char* class_name, const char* func_name, void** out_delegate) {
    if (m_coreclr_create_delegate == nullptr || m_handle == nullptr || m_domain_id == 0) {
      return false;
    }
    int error_code = m_coreclr_create_delegate(m_handle, m_domain_id, name_space, class_name, func_name, out_delegate);
    return (error_code < 0) ? false : true;
  }
}  // namespace CSharp
}  // namespace Framework
