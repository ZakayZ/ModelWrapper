#pragma once

#include "lib/condition/iface.h"
#include "lib/util/context.h"

namespace config_builder {

    class IConditionBuilder {
    public:
        virtual Condition Build(const Context& context) = 0;

        virtual ~IConditionBuilder() = default;
    };

} // namespace config_builder
