/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-10-15 09:54:16
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-12-18 11:30:24
 */

#pragma once

#if defined(__linux__)
#  include "engine/inc/log/env.h"
#  include <signal.h>
#  include <unistd.h>

#  ifdef GPERFTOOLS_CPU_MACRO
#    include <gperftools/profiler.h>
#  endif

#  ifdef GPERFTOOLS_MEMORY_MACRO
#    include <gperftools/heap-profiler.h>
#  endif

void OnSignalQuit(int signo) {
  LogError("Singal Quit");
  GServer->Quit();
}

void OnSignalHandler(int signo) {
  switch (signo) {
    case SIGUSR1: {
#  ifdef GPERFTOOLS_CPU_MACRO
      static bool cpu_start_flag = false;
      if (!cpu_start_flag) {
        cpu_start_flag = true;
        std::stringstream cpu_ss;
        cpu_ss << "cpu.prof";
        ProfilerStart(cpu_ss.str().c_str());
        std::cout << "recv sigusr1 start cpu collectio" << std::endl;
      } else {
        cpu_start_flag = false;
        ProfilerStop();
        std::cout << "recv sigusr1 stop cpu collection" << std::endl;
      }
#  endif

    } break;
    case SIGUSR2: {
#  ifdef GPERFTOOLS_MEMORY_MACRO
      static bool memory_start_flag = false;
      if (!memory_start_flag) {
        memory_start_flag = true;
        std::stringstream memory_ss;
        memory_ss << "memory";
        HeapProfilerStart(memory_ss.str().c_str());
        std::cout << "recv sigusr2 start memory collection" << std::endl;
      } else {
        memory_start_flag = false;
        HeapProfilerDump("recv sig");
        HeapProfilerStop();
        std::cout << "recv sigusr2 stop memory collection" << std::endl;
      }
#  endif
    } break;
  }
}

#endif
