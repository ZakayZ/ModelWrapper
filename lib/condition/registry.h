#pragma once

#include "registry/static_registry.h"
#include "iface.h"

namespace config_builder {

    using ConditionRegistry = StaticRegistry<Condition>;

    #define REGISTER_CONDITION(name, value) \
    [[maybe_unused]] static const bool is_condition_defined_##name = ConditionRegistry::Register(#name, value);

} // namespace config_builder
