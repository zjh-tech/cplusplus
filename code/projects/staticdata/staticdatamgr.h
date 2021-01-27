#pragma once
#include "engine/inc/common/errorstring.h"
#include <cinttypes>
#include <memory>
                     
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
                     
class StaticDataMgr 
{
public:
   ~StaticDataMgr();
                     
   const char* GetPath();
                     
   ErrorStringPtr LoadAllCfg(const char *path);
                     
private:
   bool load_cfg(const char *path, google::protobuf::Message *msg);
                     
public:
   const config::tipcfg*  GetTipCfg();
                     
   const config::tip*  GetTipByID(uint32_t id);
                     
private:
   const char*  load_path = nullptr;
                     
   config::tipcfg *tip_ptr = nullptr;
};
                     
extern shared_ptr<StaticDataMgr> GStaticDataMgr;                     
