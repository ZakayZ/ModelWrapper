#pragma once

#include "iface.h"
#include "lib/registry/static_registry.h"

namespace config_builder {

    using ConditionBuilderRegistry = StaticRegistry<IConditionBuilder*>;

} // namespace config_builder

#define REGISTER_CONDITION_BUILDER(name, value) \
[[maybe_unused]] static const bool is_condition_builder_defined_##name = config_builder::ConditionBuilderRegistry::Register(#name, value);

