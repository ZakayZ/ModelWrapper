#pragma once

#include "iface.h"
#include "lib/registry/static_registry.h"

namespace config_builder {
    using ModelRegistry = StaticRegistry<IModel*>;
} // namespace config_builder

#define REGISTER_MODEL(name, value) \
[[maybe_unused]] static const bool is_model_defined_##name = config_builder::ModelRegistry::Register(#name, value);

