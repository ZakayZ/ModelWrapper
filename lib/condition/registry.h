#pragma once

#include "lib/registry/static_registry.h"
#include "iface.h"

namespace config_builder {

    using ConditionRegistry = StaticRegistry<Condition>;

} // namespace config_builder

#define REGISTER_CONDITION(name, value) \
[[maybe_unused]] static const bool is_condition_defined_##name = config_builder::ConditionRegistry::Register(#name, value);
