#pragma once

#include "builders/context.h"
#include "condition/iface.h"

namespace config_builder {

    class IConditionBuilder {
    public:
        virtual Condition Build(const Context& context) = 0;

        virtual ~IConditionBuilder() = default;
    };

} // namespace config_builder
