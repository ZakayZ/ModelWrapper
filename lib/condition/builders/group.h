#pragma once

#include "lib/condition/group.h"
#include "lib/util/context.h"
#include "lib/util/util.h"

#include "iface.h"
#include "registry.h"

namespace config_builder {

    template <GroupType Type>
    class GroupConditionBuilder : public IConditionBuilder {
    public:
        Condition Build(const Context& context) override {
            IGroupCondition::Conditions conditions;
            for (const auto& child: context.children) {
                std::visit(overloaded{
                    [&](Condition condition) {
                        conditions.emplace_back(condition);
                    },
                    [](auto /* arg */) {
                        throw std::runtime_error("group condition must contain only conditions");
                    }
                }, child);
            }

            if constexpr (Type == GroupType::OR) {
                return ORCondition(std::move(conditions));
            } else if constexpr (Type == GroupType::AND) {
                return ANDCondition(std::move(conditions));
            }
        }
    };

    REGISTER_CONDITION_BUILDER(or, new GroupConditionBuilder<GroupType::OR>());
    REGISTER_CONDITION_BUILDER(and, new GroupConditionBuilder<GroupType::AND>());

} // namespace config_builder
