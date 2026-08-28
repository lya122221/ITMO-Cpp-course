#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <list>
#include <variant>
#include "database.hpp" 

using namespace kvdb;

class DatabaseTest : public ::testing::Test {
protected:
  DataBase db; 
};

TEST_F(DatabaseTest, UnknownCommandReturnsMonostate) {
  auto res = db.ExecuteCommand("HACKDB", std::vector<std::string>{"arg1"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res));
}

TEST_F(DatabaseTest, CommandCaseInsensitivity) {
  db.ExecuteCommand("sEt", "mykey", "myval");
  auto res1 = db.ExecuteCommand("GET", "mykey");
  auto res2 = db.ExecuteCommand("get", "mykey");
  auto res3 = db.ExecuteCommand("gEt", "mykey");
  ASSERT_TRUE(std::holds_alternative<std::string>(res1));
  EXPECT_EQ(std::get<std::string>(res1), "myval");
  ASSERT_TRUE(std::holds_alternative<std::string>(res2));
  EXPECT_EQ(std::get<std::string>(res2), "myval");
  ASSERT_TRUE(std::holds_alternative<std::string>(res3));
  EXPECT_EQ(std::get<std::string>(res3), "myval");
}

TEST_F(DatabaseTest, VariadicTemplateArgumentConversion) {
  db.ExecuteCommand("LPUSH", "templist", "string_val", 42); 
  auto len_res = db.ExecuteCommand("LLEN", "templist");
  ASSERT_TRUE(std::holds_alternative<int>(len_res));
  EXPECT_EQ(std::get<int>(len_res), 2);
  auto pop_res = db.ExecuteCommand("LPOP", "templist", 1);
  ASSERT_TRUE(std::holds_alternative<std::list<std::string>>(pop_res));
  auto popped_list = std::get<std::list<std::string>>(pop_res);
  EXPECT_EQ(popped_list.front(), "42");
}

TEST_F(DatabaseTest, DatabaseInitializationWorks) {
  auto res = db.ExecuteCommand("DBSIZE");
  ASSERT_TRUE(std::holds_alternative<int>(res));
  EXPECT_EQ(std::get<int>(res), 0);
}