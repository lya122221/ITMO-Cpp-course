#include <gtest/gtest.h>
#include <memory>
#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>
#include "set_commands.hpp"
#include "general_commands.hpp"
#include "storage.hpp"

using namespace kvdb;

class SetCommandsTest : public ::testing::Test {
protected:
  void SetUp() override {
    storage = std::make_shared<Storage>();
  }
  std::shared_ptr<Storage> storage;
};

TEST_F(SetCommandsTest, BasicSetOperations) {
  SaddCmd sadd;
  SremCmd srem;
  ScardCmd scard;
  SismemberCmd sismember;
  auto res_add = sadd.Execute(storage, {"myset", "apple", "banana", "apple", "orange"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_add));
  auto res_card = scard.Execute(storage, {"myset"});
  ASSERT_TRUE(std::holds_alternative<int>(res_card));
  EXPECT_EQ(std::get<int>(res_card), 3);
  auto res_is_apple = sismember.Execute(storage, {"myset", "apple"});
  ASSERT_TRUE(std::holds_alternative<bool>(res_is_apple));
  EXPECT_TRUE(std::get<bool>(res_is_apple));
  auto res_is_kiwi = sismember.Execute(storage, {"myset", "kiwi"});
  EXPECT_FALSE(std::get<bool>(res_is_kiwi));
  srem.Execute(storage, {"myset", "banana"});
  auto res_card_after = scard.Execute(storage, {"myset"});
  EXPECT_EQ(std::get<int>(res_card_after), 2);
}

TEST_F(SetCommandsTest, SmembersReturnsAllElements) {
  SaddCmd sadd;
  SmembersCmd smembers;
  sadd.Execute(storage, {"myset", "A", "B", "C"});
  auto res_members = smembers.Execute(storage, {"myset"});
  ASSERT_TRUE(std::holds_alternative<std::vector<std::string>>(res_members));
  auto members_vec = std::get<std::vector<std::string>>(res_members);
  EXPECT_EQ(members_vec.size(), 3);
  EXPECT_NE(std::find(members_vec.begin(), members_vec.end(), "A"), members_vec.end());
  EXPECT_NE(std::find(members_vec.begin(), members_vec.end(), "B"), members_vec.end());
  EXPECT_NE(std::find(members_vec.begin(), members_vec.end(), "C"), members_vec.end());
}

TEST_F(SetCommandsTest, SetMathematics) {
  SaddCmd sadd;
  SunionCmd sunion;
  SinterCmd sinter;
  SdiffCmd sdiff;
  sadd.Execute(storage, {"set1", "1", "2", "3"});
  sadd.Execute(storage, {"set2", "2", "3", "4"});
  sadd.Execute(storage, {"set3", "3", "5"});
  auto res_union = sunion.Execute(storage, {"set1", "set2"});
  ASSERT_TRUE(std::holds_alternative<std::unordered_set<std::string>>(res_union));
  auto union_set = std::get<std::unordered_set<std::string>>(res_union);
  EXPECT_EQ(union_set.size(), 4);
  auto res_inter = sinter.Execute(storage, {"set1", "set2", "set3"});
  ASSERT_TRUE(std::holds_alternative<std::unordered_set<std::string>>(res_inter));
  auto inter_set = std::get<std::unordered_set<std::string>>(res_inter);
  EXPECT_EQ(inter_set.size(), 1);
  EXPECT_TRUE(inter_set.contains("3"));
  auto res_diff = sdiff.Execute(storage, {"set1", "set2"});
  ASSERT_TRUE(std::holds_alternative<std::unordered_set<std::string>>(res_diff));
  auto diff_set = std::get<std::unordered_set<std::string>>(res_diff);
  EXPECT_EQ(diff_set.size(), 1);
  EXPECT_TRUE(diff_set.contains("1"));
}

TEST_F(SetCommandsTest, SmoveElements) {
  SaddCmd sadd;
  SmoveCmd smove;
  SismemberCmd sismember;
  sadd.Execute(storage, {"src_set", "item1", "item2"});
  sadd.Execute(storage, {"dest_set", "item3"});
  auto res_move = smove.Execute(storage, {"src_set", "dest_set", "item1"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_move));
  auto res_src_check = sismember.Execute(storage, {"src_set", "item1"});
  EXPECT_FALSE(std::get<bool>(res_src_check));
  auto res_dest_check = sismember.Execute(storage, {"dest_set", "item1"});
  EXPECT_TRUE(std::get<bool>(res_dest_check));
  auto res_move_fail = smove.Execute(storage, {"src_set", "dest_set", "ghost_item"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_move_fail));
}

TEST_F(SetCommandsTest, WrongTypeHandling) {
  SetCmd set_cmd;
  SaddCmd sadd;
  set_cmd.Execute(storage, {"str_key", "string_val"});
  auto res_wrongtype = sadd.Execute(storage, {"str_key", "elem"});
  EXPECT_TRUE(std::holds_alternative<std::monostate>(res_wrongtype));
}