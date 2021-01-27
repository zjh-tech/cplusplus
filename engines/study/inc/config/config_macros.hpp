/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2019-12-12 13:08:36
 * @LastEditors  : zhengjinhong
 * @LastEditTime : 2020-01-19 10:59:46
 */

#pragma once

#include <cinttypes>

namespace Framework {
namespace Config {

#define RELOAD_FLAG true
#define UNRELOAD_FLAG false

#define IMPLEMENT_CONFIG(filename)                     \
public:                                                \
  C##filename##Config* Get##filename##Config() const { \
    return m_p##filename##Cfg;                         \
  }                                                    \
                                                       \
private:                                               \
  C##filename##Config* m_p##filename##Cfg = new C##filename##Config();

//------------------------------------------------
#define IMPLEMENT_EX_CONFIG(filename)                      \
public:                                                    \
  CEx##filename##Config* GetEx##filename##Config() const { \
    return m_pEx##filename##Cfg;                           \
  }                                                        \
                                                           \
private:                                                   \
  CEx##filename##Config* m_pEx##filename##Cfg = new CEx##filename##Config();

  //------------------------------------------------

#define BIND_CONFIG(filename)                                                                  \
  csv_ss.clear();                                                                              \
  csv_ss << #filename << ".csv";                                                               \
  class_name_ss.clear();                                                                       \
  class_name_ss << "C" << #filename << "Config";                                               \
  bindInfo.Set((IConfig**)&m_p##filename##Cfg, csv_ss.str(), class_name_ss.str(), reloadflag); \
  m_file_ptr_map[#filename] = bindInfo;

  //------------------------------------------------

#define BIND_EX_CONFIG(filename, reloadflag)                                                     \
  SCsvBindInfo      bindInfo;                                                                    \
  std::stringstream csv_ss;                                                                      \
  std::stringstream class_name_ss;                                                               \
  csv_ss << #filename << ".csv";                                                                 \
  class_name_ss.clear();                                                                         \
  class_name_ss << "CEx" << #filename << "Config";                                               \
  bindInfo.Set((IConfig**)&m_pEx##filename##Cfg, csv_ss.str(), class_name_ss.str(), reloadflag); \
  m_file_ptr_map[#filename] = bindInfo;

}  // namespace Config
}  // namespace Framework
