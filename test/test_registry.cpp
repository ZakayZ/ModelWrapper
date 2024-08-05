#include <gtest/gtest.h>
#include <string>

#include "lib/registry/registry.h"
#include "lib/registry/static_registry.h"

using namespace config_builder;

namespace {
    struct DestructorChecker {
        DestructorChecker(bool& is_destructed) : is_destructed(is_destructed) {
            is_destructed = false;
        }

        ~DestructorChecker() {
            is_destructed = true;
        }

        bool& is_destructed;
    };

    static const std::string Name = "name";

    using IntRegistry = StaticRegistry<int>;

    [[maybe_unused]] static const bool is_value_defined = IntRegistry::Register(Name, 1);
}  // anonymous namespace

TEST(TestRegistry, TestRegister) {
    Registry<int> registry;
	ASSERT_TRUE(registry.Register(Name, 1)) << "first insert";
    ASSERT_FALSE(registry.Register(Name, 1)) << "not inserted";
}

TEST(TestRegistry, TestMemorySafety) {
    bool is_destructed;
    {
        Registry<DestructorChecker*> ptr_registry;
        ASSERT_TRUE(ptr_registry.Register(Name, new DestructorChecker(is_destructed))) << "first insert";
    }
    EXPECT_TRUE(is_destructed);
}

TEST(TestRegistry, TestFind) {
    Registry<int> registry;
    int value = 2;
    ASSERT_TRUE(registry.Register(Name, value)) << "first insert";

    auto registry_value = registry.Find(Name);
    ASSERT_TRUE(registry_value.has_value()) << "inserted earlier";
    EXPECT_EQ(registry_value.value(), value) << "inserted \"" << value << "\" earlier";

    ASSERT_FALSE(registry.Find("missing").has_value()) << "must be missing";
}

TEST(TestRegistry, TestStaticRegistry) {
    auto registry_value = IntRegistry::Find(Name);
    ASSERT_TRUE(registry_value.has_value()) << "statically inserted";
    EXPECT_EQ(registry_value.value(), 1) << "statically inserted \"1\"";
}
