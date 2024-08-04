#include <gtest/gtest.h>
#include <functional>

#include <COLA.hh>

#include "condition/compare.h"
#include "condition/group.h"

#include "property/registry.h"

using namespace config_builder;

namespace {
    Condition TrueCondition = [](const cola::Particle&) -> bool { return true; };
    Condition FalseCondition = [](const cola::Particle&) -> bool { return false; };
}  // anonymous namespace

TEST(TestCondition, TestORGroup) {
    EXPECT_FALSE(ORCondition(ORCondition::Conditions())(cola::Particle{})) << "empty is false";

    EXPECT_TRUE(ORCondition({
        TrueCondition,
        TrueCondition
    })(cola::Particle{})) << "all conditions are true";

    EXPECT_TRUE(ORCondition({
        TrueCondition,
        FalseCondition
    })(cola::Particle{})) << "some conditions are true";

    EXPECT_TRUE(ORCondition({
        FalseCondition,
        TrueCondition
    })(cola::Particle{})) << "order doesn't matter";

    EXPECT_FALSE(ORCondition({
        FalseCondition,
        FalseCondition
    })(cola::Particle{})) << "all conditions are false";
}

TEST(TestCondition, TestANDGroup) {
    EXPECT_TRUE(ANDCondition(ANDCondition::Conditions())(cola::Particle{})) << "empty is true";

    EXPECT_TRUE(ANDCondition({
        TrueCondition,
        TrueCondition
    })(cola::Particle{})) << "all conditions are true";

    EXPECT_FALSE(ANDCondition({
        TrueCondition,
        FalseCondition
    })(cola::Particle{})) << "some conditions are false";

    EXPECT_FALSE(ANDCondition({
        FalseCondition,
        TrueCondition
    })(cola::Particle{})) << "order doesn't matter";

    EXPECT_FALSE(ANDCondition({
        FalseCondition,
        [](const cola::Particle&) -> bool { return false; }
    })(cola::Particle{})) << "all conditions are false";
}

TEST(TestCondition, TestCompare) {
    auto nuclei = cola::AZ{2, 1};
    cola::Particle particle;
    particle.pdgCode = cola::AZToPdg(nuclei);
    const auto nuclei_thr = cola::AZ{12, 12};

    auto cmp_mass = CompareCondition<std::less<Float>>(
        std::to_string(nuclei_thr.first),
        PropertyRegistry::Find("A").value().get()
    );
    EXPECT_TRUE(cmp_mass(particle)) << "\"" << nuclei.first << "\" < \"" << nuclei_thr.first << "\"";

    auto cmp_charge = CompareCondition<std::less<Float>>(
        std::to_string(nuclei_thr.second),
        PropertyRegistry::Find("Z").value().get()
    );
    EXPECT_TRUE(cmp_charge(particle)) << "\"" << nuclei.second << "\" < \"" << nuclei_thr.second << "\"";
}
