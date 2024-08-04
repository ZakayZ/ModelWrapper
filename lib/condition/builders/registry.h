#pragma once

#include "iface.h"
#include "registry/static_registry.h"

namespace config_builder {

    using ConditionBuilderRegistry = StaticRegistry<IConditionBuilder*>;

    #define REGISTER_CONDITION_BUILDER(name, value) \
    [[maybe_unused]] static const bool is_condition_builder_defined_##name = ConditionBuilderRegistry::Register(#name, value);

} // namespace config_builder
