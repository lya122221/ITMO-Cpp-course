#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <list>
#include "list_commands.hpp"
#include "general_commands.hpp"
#include "storage.hpp"

using namespace kvdb;

class ListCommandsTest : public ::testing::Test {
protected:
  void SetUp() override {
    storage = std::make_shared<Storage>();
  }
  std::shared_ptr<Storage> storage;
};

TEST_F(ListCommandsTest, PushAndPopWithCount) {
  LpushCmd lpush;
  RpushCmd rpush;
  LpopCmd lpop;
  RpopCmd rpop;
  LlenCmd llen;
  auto res_lpush = lpush.Execute(storage, {"mylist", "a", "b"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_lpush));
  rpush.Execute(storage, {"mylist", "c", "d"});
  auto res_len = llen.Execute(storage, {"mylist"});
  ASSERT_TRUE(std::holds_alternative<int>(res_len));
  EXPECT_EQ(std::get<int>(res_len), 4);
  auto res_lpop = lpop.Execute(storage, {"mylist", "2"});
  ASSERT_TRUE(std::holds_alternative<std::list<std::string>>(res_lpop));
  auto popped_left = std::get<std::list<std::string>>(res_lpop);
  EXPECT_EQ(popped_left.size(), 2);
  EXPECT_EQ(popped_left.front(), "b");
  EXPECT_EQ(popped_left.back(), "a");
  auto res_rpop = rpop.Execute(storage, {"mylist"});
  ASSERT_TRUE(std::holds_alternative<std::list<std::string>>(res_rpop));
  auto popped_right = std::get<std::list<std::string>>(res_rpop);
  EXPECT_EQ(popped_right.size(), 1);
  EXPECT_EQ(popped_right.front(), "d");
}

TEST_F(ListCommandsTest, IndexAndSetWithNegativeValues) {
  RpushCmd rpush;
  LindexCmd lindex;
  LsetCmd lset;
  rpush.Execute(storage, {"mylist", "A", "B", "C", "D"});
  auto res_idx_pos = lindex.Execute(storage, {"mylist", "2"});
  ASSERT_TRUE(std::holds_alternative<std::string>(res_idx_pos));
  EXPECT_EQ(std::get<std::string>(res_idx_pos), "C");
  auto res_idx_neg = lindex.Execute(storage, {"mylist", "-1"});
  ASSERT_TRUE(std::holds_alternative<std::string>(res_idx_neg));
  EXPECT_EQ(std::get<std::string>(res_idx_neg), "D");
  auto res_set = lset.Execute(storage, {"mylist", "-2", "X"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_set));
  auto res_check = lindex.Execute(storage, {"mylist", "2"});
  EXPECT_EQ(std::get<std::string>(res_check), "X");
}

TEST_F(ListCommandsTest, LrangeLogic) {
  RpushCmd rpush;
  LrangeCmd lrange;
  rpush.Execute(storage, {"mylist", "0", "1", "2", "3", "4"});
  auto res_range = lrange.Execute(storage, {"mylist", "1", "3"});
  ASSERT_TRUE(std::holds_alternative<std::list<std::string>>(res_range));
  auto list_res = std::get<std::list<std::string>>(res_range);
  EXPECT_EQ(list_res.size(), 2);
  EXPECT_EQ(list_res.front(), "1");
  EXPECT_EQ(list_res.back(), "2");
  auto res_range_neg = lrange.Execute(storage, {"mylist", "-2", "5"});
  auto list_neg = std::get<std::list<std::string>>(res_range_neg);
  EXPECT_EQ(list_neg.size(), 2);
  EXPECT_EQ(list_neg.front(), "3");
}

TEST_F(ListCommandsTest, LinsertBeforeAndAfter) {
  RpushCmd rpush;
  LinsertCmd linsert;
  LrangeCmd lrange;
  rpush.Execute(storage, {"mylist", "A", "B", "C"});
  linsert.Execute(storage, {"mylist", "BEFORE", "1", "X"});
  linsert.Execute(storage, {"mylist", "AFTER", "-1", "Y"});
  auto res_all = lrange.Execute(storage, {"mylist", "0", "100"});
  auto final_list = std::get<std::list<std::string>>(res_all);
  EXPECT_EQ(final_list.size(), 5);
  auto it = final_list.begin();
  EXPECT_EQ(*it++, "A");
  EXPECT_EQ(*it++, "X");
  EXPECT_EQ(*it++, "B");
  EXPECT_EQ(*it++, "C");
  EXPECT_EQ(*it++, "Y");
}

TEST_F(ListCommandsTest, ErrorHandling) {
  SetCmd set_cmd;
  LpushCmd lpush;
  LpopCmd lpop;
  set_cmd.Execute(storage, {"str_key", "just_string"});
  auto res_wrongtype = lpush.Execute(storage, {"str_key", "val"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_wrongtype));
  auto res_empty_pop = lpop.Execute(storage, {"missing_list"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_empty_pop));
  lpush.Execute(storage, {"short_list", "A"});
  auto res_overflow_pop = lpop.Execute(storage, {"short_list", "5"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_overflow_pop));
}