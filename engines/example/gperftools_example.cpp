/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-03 17:10:43
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-03-05 18:28:03
 */

#ifdef GPERFTOOLS_CPU_MACRO
#  include <gperftools/profiler.h>
#endif

#ifdef GPERFTOOLS_MEMORY_MACRO
#  include <gperftools/heap-profiler.h>
#endif

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <sstream>

//----------------------cpu------------------------
void func1() {
  int i = 0;
  while (i < 100000) {
    ++i;
  }
}

void func2() {
  int i = 0;
  while (i < 200000) {
    ++i;
  }
}
void generate_cpu_func() {
  for (int i = 0; i < 1000; ++i) {
    func1();
    func2();
  }
}

//-------------------memory----------------------
void* create(unsigned int size) {
  return malloc(size);
}

void create_destory(unsigned int size) {
  void* p = create(size);
  free(p);
}

void generate_memory_func() {
  const int loop = 500;

  std::vector<void*> vec;
  unsigned int       mega_size = 1024 * 1024;
  for (int i = 0; i < loop; i++) {
    const unsigned int create_size = 10 * mega_size;  // 1G
    void*              p           = create(create_size);
    vec.push_back(p);

    const unsigned int create_destory_size = mega_size;
    create_destory(create_destory_size);
  }
}

int main() {
#ifdef GPERFTOOLS_CPU_MACRO
  std::cout << "gperftools cpu start" << std::endl;
  std::stringstream cpu_ss;
  cpu_ss << "cpu.prof";
  ProfilerStart(cpu_ss.str().c_str());
#endif

#ifdef GPERFTOOLS_MEMORY_MACRO
  std::cout << "gperftools memory start" << std::endl;
  std::stringstream memory_ss;
  memory_ss << "memory";
  HeapProfilerStart(memory_ss.str().c_str());
#endif

  uint32_t thread_num = 10;
  std::cout << "start " << std::endl;
  for (uint32_t i = 0; i < thread_num; ++i) {
    std::thread cpu_thread(generate_cpu_func);
    cpu_thread.join();

    std::thread memory_thread(generate_memory_func);
    memory_thread.join();
  }

#ifdef GPERFTOOLS_CPU_MACRO
  ProfilerStop();
  std::cout << "gperftools cpu end" << std::endl;
#endif

#ifdef GPERFTOOLS_MEMORY_MACRO
  HeapProfilerStop();
  std::cout << "gperftools memory end" << std::endl;
#endif
  std::cout << "end " << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(5));

  return 0;
}
