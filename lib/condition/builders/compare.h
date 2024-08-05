#pragma once

#include <functional>

#include <COLA.hh>

#include "lib/condition/compare.h"
#include "lib/property/registry.h"

#include "iface.h"
#include "registry.h"

namespace config_builder {

    template <template <typename> class Comparator>
    class CompareConditionBuilder : public IConditionBuilder {
    public:
        Condition Build(const Context& context) override {
            const auto& value = context.contents;
            auto property_it = context.attributes.find("property");
            if (property_it == context.attributes.end()) {
                throw std::runtime_error("compare tag must contain \"property\" attribute");
            }

            auto getter = PropertyRegistry::Find(property_it->second);
            if (!getter.has_value()) {
                throw std::runtime_error("unknown property: " + property_it->second);
            }
            return CompareCondition<Comparator<Float>>(value, getter.value().get());
        }
    };

    REGISTER_CONDITION_BUILDER(eq, new CompareConditionBuilder<std::equal_to>());
    REGISTER_CONDITION_BUILDER(lt, new CompareConditionBuilder<std::less>());
    REGISTER_CONDITION_BUILDER(leq, new CompareConditionBuilder<std::less_equal>());
    REGISTER_CONDITION_BUILDER(gt, new CompareConditionBuilder<std::greater>());
    REGISTER_CONDITION_BUILDER(geq, new CompareConditionBuilder<std::greater_equal>());
    REGISTER_CONDITION_BUILDER(neq, new CompareConditionBuilder<std::not_equal_to>());

} // namespace config_builder
