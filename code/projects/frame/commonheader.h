/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-11 10:22:06
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-10-14 19:10:44
 */

#pragma once

#if defined(_WIN32) || defined(_WIN64)
#  include <WinSock2.h>
#  include <windows.h>
#else
#  include <unistd.h>
#endif