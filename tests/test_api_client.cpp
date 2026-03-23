#include <gtest/gtest.h>
#include "api_client.h"
#include "serialization.h"
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class APIClientTest : public ::testing::Test {
protected:
    std::string test_file_;

    void SetUp() override {
        test_file_ = (std::filesystem::temp_directory_path() / ("test_cities_" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) + ".json")).string();
    }

    void TearDown() override {
        std::filesystem::remove(test_file_);
    }
};

TEST_F(APIClientTest, ValidateRoutes_KeepsRoutesWithLessThan3Segments) {
    std::ofstream dummy(test_file_);
    dummy << "{}";
    dummy.close();

    APIClient client("dummy_key", test_file_);
    Route r1, r2, r3;
    r1.segments.resize(2);
    r2.segments.resize(3);
    r3.segments.resize(4);
    std::vector<Route> routes = {r1, r2, r3};
    auto result = client.ValidateRoutes(routes);
    EXPECT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].segments.size(), 2);
}

TEST_F(APIClientTest, LoadCitiesFromFile_Success) {
    json cities_json = json::parse(R"({
        "Москва": "c213",
        "Санкт-Петербург": "c2"
    })");
    std::ofstream file(test_file_);
    file << cities_json.dump();
    file.close();

    APIClient client("dummy_key", test_file_);
    EXPECT_EQ(client.GetCityCode("Москва"), "c213");
    EXPECT_EQ(client.GetCityCode("Санкт-Петербург"), "c2");
}

TEST_F(APIClientTest, GetCityCode_ThrowsWhenCityNotFound) {
    std::ofstream file(test_file_);
    file << "{}";
    file.close();

    APIClient client("dummy_key", test_file_);
    EXPECT_THROW(client.GetCityCode("Москва"), std::runtime_error);
}