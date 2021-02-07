/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-13 18:53:43
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-02-07 17:10:22
 */

#pragma once
#include "asio.hpp"
#include "engine/inc/mysql/dbdefine.h"
#include "engine/inc/mysql/idbcommand.h"

namespace Framework {
class IOContextPool;
}  // namespace Framework

using namespace Framework;
using namespace std;

namespace Framework {
namespace DB {

#define TABLE_NAME_MAX_LEN 256

  class IDBConn;
  using DBConnPtr = shared_ptr<IDBConn>;

  class IDBCommand;

  class MysqlModule : public enable_shared_from_this<MysqlModule> {
  public:
    MysqlModule();
    virtual ~MysqlModule();

  public:
    bool Init(vector<DBConnSpec>& conn_specs, uint32_t table_max_count, uint32_t conn_max_count, shared_ptr<IOContextPool> processor_pool);

    bool Run(uint32_t count);

    void Stop();

    //sync
    void SyncExecCommand(uint64_t uid, ExecSqlFunc execSqlFunc, ExecSqlRecordFunc execSqlRecordFunc);

    //async
    void AsyncExecCommand(uint64_t uid, ExecSqlFunc execSqlFunc, ExecSqlRecordFunc execSqlRecordFunc);

    //hash
    uint64_t HashDBIndex(uint64_t uid);

    uint64_t HashTableIndex(uint64_t uid);

    string GetTableNameByUID(string table_name, uint64_t uid);

  private:
    DBConnPtr choose_connection(uint64_t uid);

  private:
    vector<DBConnSpec> m_conn_specs;
    uint32_t           m_table_max_count = 0;
    uint32_t           m_conn_max_count  = 0;

    vector<DBConnPtr>         m_conn_vec;
    shared_ptr<IOContextPool> m_io_context_pool = nullptr;
    asio::io_context          m_io_context;
    asio::io_context::work    m_io_worker;
  };

  extern shared_ptr<MysqlModule> GMysqlModule;

}  // namespace DB
}  // namespace Framework