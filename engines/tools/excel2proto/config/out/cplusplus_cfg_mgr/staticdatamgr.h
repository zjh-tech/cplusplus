#pragma once
#include "engine/inc/common/errorstring.h"
#include "engine/inc/common/singleton.hpp"
#include <cinttypes>
                     
using namespace std;
using namespace Framework;
                     
namespace google
{
   namespace protobuf
   {
      class Message;
   }
}
                     
namespace config
{
   class   tipcfg;
   class   tip;
}
                     
class StaticDataMgr : public Singleton<StaticDataMgr>
{
public:
   ~StaticDataMgr();
                     
   ErrorStringPtr LoadAllCfg(const char *path);
                     
private:
   bool load_cfg(const char *path, google::protobuf::Message *msg);
                     
public:
   const config::tipcfg*  GetTipCfg();
   const config::tip*  GetTipByID(uint32_t id);
                     
private:
   config::tipcfg *tip_ptr = nullptr;
};
                     
