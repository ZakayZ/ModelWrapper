#include <gtest/gtest.h>
#include <map>

#include "config_module.h"

using ParamMap = std::map<std::string, std::string>;

TEST(TestFactory, TestFactory) {
    auto params = ParamMap({{"config", "example.xml"}});
    auto factory = cola::ConfigFactory();
    auto converter = std::unique_ptr<cola::VConverter>(dynamic_cast<cola::VConverter*>(factory.create(params)));

    {
        auto results = (*converter)(std::make_unique<cola::EventData>());
        EXPECT_EQ(results->particles.size(), 0);
    }

    {
        auto data = std::make_unique<cola::EventData>();
        data->particles.push_back({});
        auto results = (*converter)(std::move(data));
        EXPECT_EQ(results->particles.size(), 1);
    }
}
