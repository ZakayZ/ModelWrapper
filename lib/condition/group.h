#pragma once

#include <vector>

#include "condition.h"

namespace config_builder {

    class IGroupCondition : public ICondition {
    public:
        using Conditions = std::vector<Condition>;

        IGroupCondition(Conditions&& conditions) : conditions_(std::move(conditions)) {}

    protected:
        Conditions conditions_;
    };

    enum class GroupType {
        OR,
        AND,
    };

    template <GroupType type>
    class GroupCondition : public IGroupCondition {};

    template <>
    class GroupCondition<GroupType::OR> : public IGroupCondition {
    public:
        using IGroupCondition::IGroupCondition;

        bool operator()(const cola::Particle& particle) override {
            for (auto& condition: conditions_) {
                if (condition(particle)) {
                    return true;
                }
            }

            return false;
        }
    };

    template <>
    class GroupCondition<GroupType::AND> : public IGroupCondition {
    public:
        using IGroupCondition::IGroupCondition;

        bool operator()(const cola::Particle& particle) override {
            for (auto& condition : conditions_) {
                if (!condition(particle)) {
                    return false;
                }
            }

            return true;
        }
    };

    using ORCondition = GroupCondition<GroupType::OR>;
    using ANDCondition = GroupCondition<GroupType::AND>;

} // namespace config_builder
