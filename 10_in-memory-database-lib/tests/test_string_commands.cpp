#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>
#include <list>
#include "string_commands.hpp"
#include "general_commands.hpp"
#include "storage.hpp"

using namespace kvdb;

class StringCommandsTest : public ::testing::Test {
protected:
  void SetUp() override {
    storage = std::make_shared<Storage>();
  }
  std::shared_ptr<Storage> storage;
};

TEST_F(StringCommandsTest, GetAndAppend) {
  SetCmd set_cmd;
  GetCmd get_cmd;
  AppendCmd append_cmd;
  set_cmd.Execute(storage, {"my_str", "Hello"});
  auto res_get = get_cmd.Execute(storage, {"my_str"});
  ASSERT_TRUE(std::holds_alternative<std::string>(res_get));
  EXPECT_EQ(std::get<std::string>(res_get), "Hello");
  auto res_append = append_cmd.Execute(storage, {"my_str", " World"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_append));
  auto res_get_after = get_cmd.Execute(storage, {"my_str"});
  EXPECT_EQ(std::get<std::string>(res_get_after), "Hello World");
}

TEST_F(StringCommandsTest, StrlenCommand) {
  SetCmd set_cmd;
  StrlenCmd strlen_cmd;
  set_cmd.Execute(storage, {"my_str", "1234567890"});
  auto res_len = strlen_cmd.Execute(storage, {"my_str"});
  ASSERT_TRUE(std::holds_alternative<int>(res_len));
  EXPECT_EQ(std::get<int>(res_len), 10);
}

TEST_F(StringCommandsTest, MissingKeyHandling) {
  GetCmd get_cmd;
  AppendCmd append_cmd;
  StrlenCmd strlen_cmd;
  auto res_get = get_cmd.Execute(storage, {"missing_key"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_get));
  auto res_append = append_cmd.Execute(storage, {"missing_key", "text"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_append));
  auto res_len = strlen_cmd.Execute(storage, {"missing_key"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_len));
}

TEST_F(StringCommandsTest, WrongTypeHandling) {
  GetCmd get_cmd;
  std::list<std::string> dummy_list = {"a", "b"};
  storage->Set("list_key", dummy_list);
  auto res_get = get_cmd.Execute(storage, {"list_key"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_get));
}

TEST_F(StringCommandsTest, InvalidArgumentsCount) {
  GetCmd get_cmd;
  AppendCmd append_cmd;
  auto res_get = get_cmd.Execute(storage, {});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_get));
  auto res_append = append_cmd.Execute(storage, {"my_str"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_append));
}