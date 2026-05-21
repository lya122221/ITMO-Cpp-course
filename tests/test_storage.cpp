#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include "storage.hpp"

using namespace kvdb;

class StorageTest : public ::testing::Test {
protected:
  void SetUp() override {
    storage = std::make_unique<Storage>();
  }
  std::unique_ptr<Storage> storage;
};

TEST_F(StorageTest, BasicCrudOperations) {
  EXPECT_TRUE(storage->Set("my_string", std::string("hello world")));
  EXPECT_TRUE(storage->IsExist("my_string"));
  auto res = storage->Get("my_string");
  ASSERT_TRUE(res.has_value());
  EXPECT_EQ(std::get<std::string>(res.value()), "hello world");
  auto res_missing = storage->Get("ghost_key");
  EXPECT_FALSE(res_missing.has_value());
  EXPECT_TRUE(storage->Del("my_string"));
  EXPECT_FALSE(storage->IsExist("my_string"));
  EXPECT_FALSE(storage->Del("my_string"));
}

TEST_F(StorageTest, ExpirationLogic) {
  auto ttl = std::chrono::steady_clock::now() + std::chrono::milliseconds(50);
  storage->Set("temp_key", std::string("im_alive"), ttl);
  EXPECT_TRUE(storage->IsExist("temp_key"));
  auto res = storage->Get("temp_key");
  EXPECT_TRUE(res.has_value());
  std::this_thread::sleep_for(std::chrono::milliseconds(60));
  auto res_expired = storage->Get("temp_key");
  EXPECT_FALSE(res_expired.has_value());
  EXPECT_EQ(res_expired.error(), "(error) Key expired");
}

TEST_F(StorageTest, TtlCalculation) {
  auto ttl = std::chrono::steady_clock::now() + std::chrono::seconds(2);
  storage->Set("timer_key", std::string("tick_tock"), ttl);
  auto remaining_ttl = storage->GetTtl("timer_key");
  EXPECT_GE(remaining_ttl.count(), 1);
  EXPECT_LE(remaining_ttl.count(), 2);
}

TEST_F(StorageTest, KeysByPatternMatching) {
  storage->Set("user:1000", std::string("Alice"));
  storage->Set("user:1001", std::string("Bob"));
  storage->Set("admin:2000", std::string("Charlie"));
  storage->Set("system_config", std::string("on"));
  auto users = storage->GetKeysByPattern("user:*");
  EXPECT_EQ(users.size(), 2);
  auto configs = storage->GetKeysByPattern("*config");
  EXPECT_EQ(configs.size(), 1);
  EXPECT_EQ(configs[0], "system_config");
  auto admins = storage->GetKeysByPattern("?dmin:*");
  EXPECT_EQ(admins.size(), 1);
  EXPECT_EQ(admins[0], "admin:2000");
  auto all = storage->GetKeysByPattern("*");
  EXPECT_EQ(all.size(), 4);
}

TEST_F(StorageTest, MemoryLimitsAndTracking) {
  storage->SetMaxMem(200);
  EXPECT_EQ(storage->GetMaxMem(), 200);
  bool is_set = storage->Set("short_key", std::string("tiny"));
  EXPECT_TRUE(is_set);
  size_t mem_used = storage->GetKeyMemoryUsage("short_key");
  EXPECT_GT(mem_used, 0);
  std::string huge_str(1000, 'X'); 
  bool is_huge_set = storage->Set("huge_key", huge_str);
  EXPECT_FALSE(is_huge_set);
  EXPECT_FALSE(storage->IsExist("huge_key"));
}

TEST_F(StorageTest, FlushAndSize) {
  EXPECT_EQ(storage->GetSize(), 0);
  storage->Set("k1", std::string("v1"));
  storage->Set("k2", std::string("v2"));
  EXPECT_EQ(storage->GetSize(), 2);
  storage->Flush();
  EXPECT_EQ(storage->GetSize(), 0);
  EXPECT_FALSE(storage->IsExist("k1"));
}