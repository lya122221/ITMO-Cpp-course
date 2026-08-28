#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <algorithm>
#include "general_commands.hpp"
#include "storage.hpp"

using namespace kvdb;

class GeneralCommandsTest : public ::testing::Test {
protected:
  void SetUp() override {
    storage = std::make_shared<Storage>();
  }
  std::shared_ptr<Storage> storage;
};

TEST_F(GeneralCommandsTest, SetAndTypeCommands) {
  SetCmd set_cmd;
  TypeCmd type_cmd;
  auto res_set = set_cmd.Execute(storage, {"mykey", "myval"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_set));
  auto res_type = type_cmd.Execute(storage, {"mykey"});
  ASSERT_TRUE(std::holds_alternative<std::string>(res_type));
  EXPECT_EQ(std::get<std::string>(res_type), "string");
  auto res_type_none = type_cmd.Execute(storage, {"missing_key"});
  ASSERT_TRUE(std::holds_alternative<std::string>(res_type_none));
  EXPECT_EQ(std::get<std::string>(res_type_none), "none");
}

TEST_F(GeneralCommandsTest, ExistsAndDelCommands) {
  SetCmd set_cmd;
  ExistsCmd exists_cmd;
  DelCmd del_cmd;
  set_cmd.Execute(storage, {"key1", "val1"});
  set_cmd.Execute(storage, {"key2", "val2"});
  auto res_exists_all = exists_cmd.Execute(storage, {"key1", "key2"});
  ASSERT_TRUE(std::holds_alternative<bool>(res_exists_all));
  EXPECT_TRUE(std::get<bool>(res_exists_all));
  auto res_exists_partial = exists_cmd.Execute(storage, {"key1", "key_missing"});
  ASSERT_TRUE(std::holds_alternative<std::vector<std::string>>(res_exists_partial));
  auto missing = std::get<std::vector<std::string>>(res_exists_partial);
  EXPECT_EQ(missing.size(), 1);
  EXPECT_EQ(missing[0], "key_missing");
  auto res_del = del_cmd.Execute(storage, {"key1"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_del));
  auto res_exists_after = exists_cmd.Execute(storage, {"key1"});
  EXPECT_TRUE(std::holds_alternative<std::vector<std::string>>(res_exists_after));
}

TEST_F(GeneralCommandsTest, ExpireAndTtlCommands) {
  SetCmd set_cmd;
  ExpireCmd expire_cmd;
  TtlCmd ttl_cmd;
  set_cmd.Execute(storage, {"temp_key", "val"});
  auto res_expire = expire_cmd.Execute(storage, {"temp_key", "2"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_expire));
  auto res_ttl = ttl_cmd.Execute(storage, {"temp_key"});
  ASSERT_TRUE(std::holds_alternative<int>(res_ttl));
  int ttl_val = std::get<int>(res_ttl);
  EXPECT_TRUE(ttl_val == 1 || ttl_val == 2);
}

TEST_F(GeneralCommandsTest, KeysCommandPatterns) {
  SetCmd set_cmd;
  KeysCmd keys_cmd;
  set_cmd.Execute(storage, {"user:1", "Alex"});
  set_cmd.Execute(storage, {"user:2", "Bob"});
  set_cmd.Execute(storage, {"config", "on"});
  auto res_all = keys_cmd.Execute(storage, {"*"});
  ASSERT_TRUE(std::holds_alternative<std::vector<std::string>>(res_all));
  EXPECT_EQ(std::get<std::vector<std::string>>(res_all).size(), 3);
  auto res_users = keys_cmd.Execute(storage, {"user:*"});
  ASSERT_TRUE(std::holds_alternative<std::vector<std::string>>(res_users));
  auto users = std::get<std::vector<std::string>>(res_users);
  EXPECT_EQ(users.size(), 2);
  EXPECT_TRUE(std::find(users.begin(), users.end(), "user:1") != users.end());
  EXPECT_TRUE(std::find(users.begin(), users.end(), "user:2") != users.end());
}

TEST_F(GeneralCommandsTest, AdminAndMemoryCommands) {
  SetCmd set_cmd;
  DbSizeCmd dbsize_cmd;
  FlushdbCmd flushdb_cmd;
  ConfigCmd config_cmd;
  MemUsageCmd mem_cmd;
  set_cmd.Execute(storage, {"k1", "v1"});
  set_cmd.Execute(storage, {"k2", "v2"});
  auto res_size = dbsize_cmd.Execute(storage, {});
  ASSERT_TRUE(std::holds_alternative<int>(res_size));
  EXPECT_EQ(std::get<int>(res_size), 2);
  auto res_mem = mem_cmd.Execute(storage, {"USAGE", "k1"});
  ASSERT_TRUE(std::holds_alternative<int>(res_mem));
  EXPECT_GT(std::get<int>(res_mem), 0);
  config_cmd.Execute(storage, {"SET", "maxmemory", "1048576"});
  auto res_conf_get = config_cmd.Execute(storage, {"GET", "maxmemory"});
  ASSERT_TRUE(std::holds_alternative<int>(res_conf_get));
  EXPECT_EQ(std::get<int>(res_conf_get), 1048576);
  flushdb_cmd.Execute(storage, {});
  auto res_size_after = dbsize_cmd.Execute(storage, {});
  EXPECT_EQ(std::get<int>(res_size_after), 0);
}

TEST_F(GeneralCommandsTest, InvalidArgumentsCount) {
  SetCmd set_cmd;
  TypeCmd type_cmd;
  auto res_set_err = set_cmd.Execute(storage, {"only_one_arg"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_set_err));
  auto res_type_err = type_cmd.Execute(storage, {"arg1", "arg2"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_type_err));
}