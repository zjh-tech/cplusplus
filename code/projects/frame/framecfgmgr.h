/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-15 16:04:37
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-11-23 17:52:09
 */

#pragma once

#include "engine/inc/common/errorstring.h"

using namespace Framework;

class FrameCfgMgr {
public:
  static ErrorStringPtr LoadServerCfg(const char* path);
};