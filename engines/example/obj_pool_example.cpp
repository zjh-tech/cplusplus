/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-22 20:47:05
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2020-04-24 17:56:07
 */

#include "engine/inc/log/logger.h"
#include "engine/inc/object_pool/env.h"
#include <mutex>
#include <vector>

using namespace Framework;

struct CTest {
public:
  CTest(int32_t x, int32_t y) {
    m_x = x;
    m_y = y;
  }
  ~CTest() {
    m_x = 0;
    m_y = 0;
  }

  int32_t m_x = 0;
  int32_t m_y = 0;
};

int main(void) {
  LOG_INIT("obj_pool_example", true, "debug", true);

  GObjectPoolMgr   = std::make_shared<ObjectPoolMgr>(false);
  auto   test_pool = GObjectPoolMgr->GetObjectPool<CTest>();
  CTest* pTest     = test_pool->FetchObj(1, 2);
  GObjectPoolMgr->Monitor();
  //pTest -= 1; //invalid adderess
  //test_pool->ReleaseObj(pTest); //many timer release object
  test_pool->ReleaseObj(pTest);
  GObjectPoolMgr->GC();
  GObjectPoolMgr->Monitor();

  LOG_RELEASE();
}
