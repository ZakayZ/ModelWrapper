#pragma once

#include "iface.h"
#include "lib/registry/static_registry.h"

namespace config_builder {
    using ModelBuilderRegistry = StaticRegistry<IModelBuilder*>;
} // namespace config_builder

#define REGISTER_MODEL_BUILDER(name, value) \
[[maybe_unused]] static const bool is_model_builder_defined_##name = config_builder::ModelBuilderRegistry::Register(#name, value);

