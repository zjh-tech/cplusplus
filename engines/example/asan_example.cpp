/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-03-02 14:27:38
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-03-02 19:01:43
 */

#include <iostream>
#include <stdlib.h>
int asan_use_after_free() {
  std::cout << "asan use_after_free" << std::endl;
  char* x = (char*)malloc(10 * sizeof(char*));
  free(x);
  return x[5];
}

int* asan_leak() {
  std::cout << "asan leak" << std::endl;
  int* p = new int(5);
  return p;
}

int asan_stack_buffer_underflow() {
  std::cout << "asan stack_buffer_underflow" << std::endl;
  int a[7] = {0};
  int b    = a[10];
  return b;
}

void asan_double_free() {
  std::cout << "asan double_free" << std::endl;
  char* x = (char*)malloc(10 * sizeof(char*));
  free(x);
  free(x);
}

int main(void) {
  std::cout << "asan start" << std::endl;

  asan_use_after_free();
  //asan_leak();
  //asan_stack_buffer_underflow();
  //asan_double_free();

  std::cout << "asan end" << std::endl;
  return 0;
}