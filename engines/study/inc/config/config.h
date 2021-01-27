/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-06 16:40:20
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-03-06 16:49:15
 */
/****************************************
code by tool general make ,can not modify
*****************************************/
                     
#pragma once
#include "engine/inc/common/noncopyable.hpp"
#include "engine/inc/config/config_template.hpp"
                     
namespace Framework { 
namespace Config { 
                     
class CCsvParser;
                     
class CHeroAttribute { 
public:
      void FillAttribute(const CCsvParser& parser, uint32_t row);

public:
      int32_t  id  = 0;
      std::string  name  = "";
      bool  flag  = false;
      int8_t  skill_id_1  = 0;
      uint8_t  skill_id_2  = 0;
      int16_t  skill_id_3  = 0;
      uint16_t  skill_id_4  = 0;
      int32_t  skill_id_5  = 0;
      uint32_t  skill_id_6  = 0;
      int64_t  skill_id_7  = 0;
      uint64_t  skill_id_8  = 0;
      std::string  effect  = "";
 } ;
                     
class CHeroConfig : public CConfigTemplate<CHeroAttribute,uint32_t>,Noncopyable { 
 } ;
                     
class CTestAttribute { 
public:
      void FillAttribute(const CCsvParser& parser, uint32_t row);

public:
      int32_t  id  = 0;
      std::string  name  = "";
      bool  flag  = false;
      int8_t  skill_id_1  = 0;
      uint8_t  skill_id_2  = 0;
      int16_t  skill_id_3  = 0;
      uint16_t  skill_id_4  = 0;
      int32_t  skill_id_5  = 0;
      uint32_t  skill_id_6  = 0;
      int64_t  skill_id_7  = 0;
      uint64_t  skill_id_8  = 0;
      std::string  effect  = "";
 } ;
                     
class CTestConfig : public CConfigTemplate<CTestAttribute,uint32_t>,Noncopyable { 
 } ;
                     
} // namespace Config
} // namespace Framework
