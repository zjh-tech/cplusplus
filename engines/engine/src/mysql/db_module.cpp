/*
 * @Descripttion: 
 * @Author: zhengjinhong
 * @Date: 2020-04-13 18:53:26
 * @LastEditors: zhengjinhong
 * @LastEditTime: 2021-01-07 11:51:02
 */
#include "engine/inc/mysql/db_module.h"
#include "engine/inc/common/iocontextpool.h"
#include "engine/inc/log/env.h"
#include "engine/inc/mysql/db_conn.h"
#include "engine/inc/mysql/mysqlcommand.hpp"
#include <iomanip>
#include <iostream>

namespace Framework {
namespace DB {

  shared_ptr<MysqlModule> GMysqlModule = nullptr;

  MysqlModule::MysqlModule() {
  }

  MysqlModule::~MysqlModule() {
    m_conn_vec.clear();
    m_io_context_pool = nullptr;
  }

  bool MysqlModule::Init(vector<DBConnSpec>& conn_specs, uint32_t table_max_count, uint32_t conn_max_count, shared_ptr<IOContextPool> io_context_pool) {
    m_conn_specs      = conn_specs;
    m_table_max_count = table_max_count;
    m_conn_max_count  = conn_max_count;

    if (m_conn_specs.size() != conn_max_count) {
      LogError("[DB] Mysql ConnMaxCount No Match");
      return false;
    }

    m_io_context_pool = io_context_pool;

    for (uint32_t i = 0; i < m_conn_max_count; ++i) {
      MySQLConnPtr mysql_conn_ptr = make_shared<MySQLConn>(m_conn_specs[i], io_context_pool->GetNextIoContext());
      assert(mysql_conn_ptr);
      if (!mysql_conn_ptr->Connect()) {
        LogError("[DB]  Connect {} {} Error", m_conn_specs[i].Host, m_conn_specs[i].DbName);
        return false;
      }

      m_conn_vec.push_back(mysql_conn_ptr);
    }

    return true;
  }

  bool MysqlModule::Run(uint32_t count) {
    if (m_io_context.poll_one() == 0) {
      return false;
    }

    for (uint32_t i = 0; i < count; ++i) {
      if (m_io_context.poll_one() == 0) {
        break;
      }
    }

    return true;
  }

  void MysqlModule::Stop() {
    m_io_context.stop();
    LogInfoA("[DB] Stop");
  }

  void MysqlModule::SyncExecCommand(uint64_t uid, ExecSqlFunc execSqlFunc, ExecSqlRecordFunc execSqlRecordFunc) {
    MySQLConnPtr conn = choose_connection(uid);
    if (conn == nullptr) {
      LogErrorA("[DB] sync_execute choose_connection error uid = {} ", uid);
      return;
    }

    auto command = make_shared<MysqlCommand>(execSqlFunc, execSqlRecordFunc);
    command->OnExecuteSql(conn);
    command->OnExecuted();
  }

  void MysqlModule::AsyncExecCommand(uint64_t uid, ExecSqlFunc execSqlFunc, ExecSqlRecordFunc execSqlRecordFunc) {
    MySQLConnPtr conn = choose_connection(uid);
    if (conn == nullptr) {
      LogErrorA("[DB] async_execute choose_connection error uid = {} ", uid);
      return;
    }

    auto command = make_shared<MysqlCommand>(execSqlFunc, execSqlRecordFunc);
    asio::post(conn->GetIoContext(), [this, conn, command]() {
      command->OnExecuteSql(conn);
      asio::post(m_io_context, [this, command]() {
        command->OnExecuted();
      });
    });
  }

  MySQLConnPtr MysqlModule::choose_connection(uint64_t uid) {
    return m_conn_vec[HashDBIndex(uid)];
  }

  uint64_t MysqlModule::HashDBIndex(uint64_t uid) {
    return uid % m_conn_max_count;
  }

  uint64_t MysqlModule::HashTableIndex(uint64_t uid) {
    return uid % m_table_max_count;
  }

  string MysqlModule::GetTableNameByUID(string table_name, uint64_t uid) {
    uint64_t     table_index = HashTableIndex(uid);
    stringstream ss;
    ss << table_name << "_" << setw(2) << setfill('0') << table_index;
    return ss.str().c_str();
  }

}  // namespace DB
}  // namespace Framework