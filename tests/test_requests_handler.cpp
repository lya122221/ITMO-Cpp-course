#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "mock_api_client.h"
#include "mock_cache.h"
#include "requests_handler.h"
#include <sstream>

using ::testing::_;
using ::testing::Return;

class CinRedirect {
public:
  CinRedirect(const std::string& input) : fake_(input) {
    old_ = std::cin.rdbuf(fake_.rdbuf());
  }
  ~CinRedirect() { std::cin.rdbuf(old_); }
private:
  std::istringstream fake_;
  std::streambuf* old_;
};

class CoutSilencer {
public:
  CoutSilencer() { old_ = std::cout.rdbuf(sink_.rdbuf()); }
  ~CoutSilencer() { std::cout.rdbuf(old_); }
private:
  std::ostringstream sink_;
  std::streambuf* old_;
};

TEST(HandleRequestsTest, CacheHit_APINotCalled) {
  MockAPIClient mock_api;
  MockCache mock_cache;

  Data cached;
  cached.from_city = "Москва";
  cached.from_code = "c213";
  cached.to_city = "Казань";
  cached.to_code = "c43";
  cached.date = "2025-01-15";
  cached.total = 3;

  CinRedirect cin("Москва\nКазань\n2025-01-15\nn\n");
  CoutSilencer silence;

  EXPECT_CALL(mock_api, GetCityCode("Москва")).WillOnce(Return("c213"));
  EXPECT_CALL(mock_api, GetCityCode("Казань")).WillOnce(Return("c43"));
  EXPECT_CALL(mock_cache, Get(_)).WillOnce(Return(cached));
  EXPECT_CALL(mock_api, GetDataFromRequest(_)).Times(0);
  EXPECT_CALL(mock_cache, Put(_)).Times(0);

  RequestsHandler handler(mock_api, mock_cache);
  EXPECT_TRUE(handler.HandleRequests());
}

TEST(HandleRequestsTest, CacheMiss_FetchesFromAPI_PutsInCache) {
  MockAPIClient mock_api;
  MockCache mock_cache;

  Data api_data;
  api_data.from_city = "Москва";
  api_data.from_code = "c213";
  api_data.to_city = "Казань";
  api_data.to_code = "c43";
  api_data.date = "2025-01-15";
  api_data.total = 5;

  CinRedirect cin("Москва\nКазань\n2025-01-15\nn\n");
  CoutSilencer silence;

  EXPECT_CALL(mock_api, GetCityCode("Москва")).WillOnce(Return("c213"));
  EXPECT_CALL(mock_api, GetCityCode("Казань")).WillOnce(Return("c43"));
  EXPECT_CALL(mock_cache, Get(_)).WillOnce(Return(std::nullopt));
  EXPECT_CALL(mock_api, GetDataFromRequest(_)).WillOnce(Return(api_data));
  EXPECT_CALL(mock_cache, Put(_)).Times(1);

  RequestsHandler handler(mock_api, mock_cache);
  EXPECT_TRUE(handler.HandleRequests());
}

TEST(HandleRequestsTest, APIFailure_ReturnsFalse) {
  MockAPIClient mock_api;
  MockCache mock_cache;

  CinRedirect cin("Москва\nКазань\n2025-01-15\n");
  CoutSilencer silence;

  EXPECT_CALL(mock_api, GetCityCode("Москва")).WillOnce(Return("c213"));
  EXPECT_CALL(mock_api, GetCityCode("Казань")).WillOnce(Return("c43"));
  EXPECT_CALL(mock_cache, Get(_)).WillOnce(Return(std::nullopt));
  EXPECT_CALL(mock_api, GetDataFromRequest(_)).WillOnce(Return(std::nullopt));

  RequestsHandler handler(mock_api, mock_cache);
  EXPECT_FALSE(handler.HandleRequests());
}

TEST(HandleRequestsTest, TwoRequests_ThenExit) {
  MockAPIClient mock_api;
  MockCache mock_cache;

  Data d1;
  d1.from_code = "c213"; d1.to_code = "c43";
  d1.from_city = "Москва"; d1.to_city = "Казань";
  d1.date = "2025-01-15"; d1.total = 2;

  Data d2;
  d2.from_code = "c43"; d2.to_code = "c213";
  d2.from_city = "Казань"; d2.to_city = "Москва";
  d2.date = "2025-01-20"; d2.total = 1;

  CinRedirect cin(
    "Москва\nКазань\n2025-01-15\n"
    "y\n"
    "Казань\nМосква\n2025-01-20\n"
    "n\n"
  );
  CoutSilencer silence;

  EXPECT_CALL(mock_api, GetCityCode("Москва")).WillRepeatedly(Return("c213"));
  EXPECT_CALL(mock_api, GetCityCode("Казань")).WillRepeatedly(Return("c43"));

  EXPECT_CALL(mock_cache, Get(_)).WillOnce(Return(std::nullopt)).WillOnce(Return(d2));

  EXPECT_CALL(mock_api, GetDataFromRequest(_)).WillOnce(Return(d1));

  EXPECT_CALL(mock_cache, Put(_)).Times(1);

  RequestsHandler handler(mock_api, mock_cache);
  EXPECT_TRUE(handler.HandleRequests());
}

TEST(HandleRequestsTest, UnknownCity_EmptyCode_APIFails) {
  MockAPIClient mock_api;
  MockCache mock_cache;

  CinRedirect cin("Атлантида\nКазань\n2025-01-15\n");
  CoutSilencer silence;

  EXPECT_CALL(mock_api, GetCityCode("Атлантида")).WillOnce(Return(""));
  EXPECT_CALL(mock_api, GetCityCode("Казань")).WillOnce(Return("c43"));
  EXPECT_CALL(mock_cache, Get(_)).WillOnce(Return(std::nullopt));
  EXPECT_CALL(mock_api, GetDataFromRequest(_)).WillOnce(Return(std::nullopt));

  RequestsHandler handler(mock_api, mock_cache);
  EXPECT_FALSE(handler.HandleRequests());
}