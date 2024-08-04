#include <gtest/gtest.h>
#include "registry_all.h"

using namespace config_builder;

TEST(TestDefaultRegistry, TestConditionBuilderRegistry) {
    EXPECT_TRUE(ConditionBuilderRegistry::Find("or").has_value());
    EXPECT_TRUE(ConditionBuilderRegistry::Find("and").has_value());

    EXPECT_TRUE(ConditionBuilderRegistry::Find("eq").has_value());
    EXPECT_TRUE(ConditionBuilderRegistry::Find("lt").has_value());
    EXPECT_TRUE(ConditionBuilderRegistry::Find("leq").has_value());
    EXPECT_TRUE(ConditionBuilderRegistry::Find("geq").has_value());
    EXPECT_TRUE(ConditionBuilderRegistry::Find("gt").has_value());
    EXPECT_TRUE(ConditionBuilderRegistry::Find("neq").has_value());
}

TEST(TestDefaultRegistry, TestPropertyRegistry) {
    EXPECT_TRUE(PropertyRegistry::Find("A").has_value());
    EXPECT_TRUE(PropertyRegistry::Find("Z").has_value());
}
