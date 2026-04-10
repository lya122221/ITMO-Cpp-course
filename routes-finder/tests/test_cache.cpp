#include <gtest/gtest.h>
#include "cache.h"
#include <filesystem>
#include <fstream>
#include <thread>
#include <string>
#include <chrono>

class CacheTest : public ::testing::Test {
protected:
  std::string test_file_;

  void SetUp() override {
    test_file_ = (std::filesystem::temp_directory_path() / ("test_cache_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".json")).string();
  }

  void TearDown() override {
    std::filesystem::remove(test_file_);
  }

  Data make_data(const std::string& from, const std::string& to, const std::string& date) {
    Data d;
    d.from_city = from;
    d.from_code = "c_" + from;
    d.to_city = to;
    d.to_code = "c_" + to;
    d.date = date;
    d.total = 1;
    return d;
  }
};

TEST_F(CacheTest, EmptyCacheReturnsNullopt) {
  Cache cache(10, 3600, test_file_);

  Request req{"c_Москва", "c_Казань", "2025-01-15"};
  auto result = cache.Get(req);

  EXPECT_FALSE(result.has_value());
}

TEST_F(CacheTest, PutThenGet) {
  Cache cache(10, 3600, test_file_);

  Data data = make_data("Москва", "Казань", "2025-01-15");
  cache.Put(data);

  Request req{"c_Москва", "c_Казань", "2025-01-15"};
  auto result = cache.Get(req);

  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->from_city, "Москва");
  EXPECT_EQ(result->to_city, "Казань");
  EXPECT_EQ(result->date, "2025-01-15");
}

TEST_F(CacheTest, DifferentKeyReturnsNullopt) {
  Cache cache(10, 3600, test_file_);

  Data data = make_data("Москва", "Казань", "2025-01-15");
  cache.Put(data);

  Request req{"c_Москва", "c_Самара", "2025-01-15"};
  auto result = cache.Get(req);

  EXPECT_FALSE(result.has_value());
}

TEST_F(CacheTest, EvictsOldestWhenFull) {
  Cache cache(2, 3600, test_file_);

  Data d1 = make_data("Москва", "Казань", "2025-01-01");
  Data d2 = make_data("Москва", "Самара", "2025-01-02");
  Data d3 = make_data("Москва", "Сочи",   "2025-01-03");

  cache.Put(d1);
  cache.Put(d2);
  cache.Put(d3);

  EXPECT_FALSE(cache.Get({"c_Москва", "c_Казань", "2025-01-01"}).has_value());

  EXPECT_TRUE(cache.Get({"c_Москва", "c_Самара", "2025-01-02"}).has_value());
  EXPECT_TRUE(cache.Get({"c_Москва", "c_Сочи",   "2025-01-03"}).has_value());
}

TEST_F(CacheTest, GetMovesToFrontPreventingEviction) {
  Cache cache(2, 3600, test_file_);

  Data d1 = make_data("Москва", "Казань", "2025-01-01");
  Data d2 = make_data("Москва", "Самара", "2025-01-02");

  cache.Put(d1);
  cache.Put(d2);

  cache.Get({"c_Москва", "c_Казань", "2025-01-01"});

  Data d3 = make_data("Москва", "Сочи", "2025-01-03");
  cache.Put(d3);

  EXPECT_TRUE(cache.Get({"c_Москва", "c_Казань", "2025-01-01"}).has_value());
  EXPECT_FALSE(cache.Get({"c_Москва", "c_Самара", "2025-01-02"}).has_value());
}

TEST_F(CacheTest, MultiplePutAndGet) {
  Cache cache(10, 3600, test_file_);

  for (int i = 0; i < 5; ++i) {
    Data d = make_data("City" + std::to_string(i), "City" + std::to_string(i + 10), "2025-01-" + std::to_string(10 + i));
    cache.Put(d);
  }

  for (int i = 0; i < 5; ++i) {
    Request req{"c_City" + std::to_string(i), "c_City" + std::to_string(i + 10), "2025-01-" + std::to_string(10 + i)};
    auto result = cache.Get(req);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->from_city, "City" + std::to_string(i));
  }
}

TEST_F(CacheTest, SurvivesRestart) {
  {
    Cache cache(10, 3600, test_file_);
    Data data = make_data("Москва", "Казань", "2025-01-15");
    cache.Put(data);
  }

  {
    Cache cache(10, 3600, test_file_);
    Request req{"c_Москва", "c_Казань", "2025-01-15"};
    auto result = cache.Get(req);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result->from_city, "Москва");
  }
}

TEST_F(CacheTest, ExpiredEntriesNotLoadedFromFile) {
  {
    Cache cache(10, 1, test_file_);
    Data data = make_data("Москва", "Казань", "2025-01-15");
    cache.Put(data);
  }

  std::this_thread::sleep_for(std::chrono::seconds(2));

  {
    Cache cache(10, 1, test_file_);
    Request req{"c_Москва", "c_Казань", "2025-01-15"};
    auto result = cache.Get(req);

    EXPECT_FALSE(result.has_value());
  }
}

TEST_F(CacheTest, HandlesCorruptedFile) {
  {
    std::ofstream file(test_file_);
    file << "NOT VALID JSON {{{";
  }

  Cache cache(10, 3600, test_file_);

  Request req{"c_A", "c_B", "2025-01-01"};
  EXPECT_FALSE(cache.Get(req).has_value());
}

TEST_F(CacheTest, HandlesEmptyFile) {
  {
    std::ofstream file(test_file_);
    file << "";
  }

  Cache cache(10, 3600, test_file_);
  Request req{"c_A", "c_B", "2025-01-01"};
  EXPECT_FALSE(cache.Get(req).has_value());
}

TEST_F(CacheTest, ConstructorWithCustomPath) {
  {
    Cache cache(10, 3600, test_file_);
    Data data = make_data("A", "B", "2025-01-01");
    cache.Put(data);
  }

  std::ifstream file(test_file_);
  ASSERT_TRUE(file.good());
  json j = json::parse(file);
  EXPECT_TRUE(j.is_array());
  EXPECT_EQ(j.size(), 1);
}

TEST_F(CacheTest, LoadCacheFromCorruptedFile_CreatesNewFile) {
  std::ofstream file(test_file_);
  file << "NOT A JSON";
  file.close();

  Cache cache(10, 3600, test_file_);
  Request req{"c_A", "c_B", "2025-01-01"};
  EXPECT_FALSE(cache.Get(req).has_value());

  std::ifstream new_file(test_file_);
  json j = json::parse(new_file);
  EXPECT_TRUE(j.is_array());
  EXPECT_EQ(j.size(), 0);
}

TEST_F(CacheTest, SaveOnlyNonExpiredEntries) {
  {
    Cache cache(10, 1, test_file_);
    Data d1 = make_data("A", "B", "2025-01-01");
    Data d2 = make_data("C", "D", "2025-01-02");
    cache.Put(d1);
    cache.Put(d2);
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  Cache cache2(10, 3600, test_file_);
  EXPECT_FALSE(cache2.Get({"c_A", "c_B", "2025-01-01"}).has_value());
  EXPECT_FALSE(cache2.Get({"c_C", "c_D", "2025-01-02"}).has_value());
}